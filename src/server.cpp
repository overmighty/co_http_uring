// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/server.hpp"

#include "co_http_uring/connection.hpp"
#include "co_http_uring/connection_reader.hpp"
#include "co_http_uring/connection_writer.hpp"
#include "co_http_uring/error.hpp"
#include "co_http_uring/eventfd.hpp"
#include "co_http_uring/io_uring.hpp"
#include "co_http_uring/request.hpp"
#include "co_http_uring/response_writer.hpp"
#include "co_http_uring/socket_address.hpp"
#include "co_http_uring/task.hpp"
#include "co_http_uring/tcp_socket.hpp"
#include "co_http_uring/types.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

extern "C" {
#include <sys/eventfd.h>
}

using namespace std::literals::chrono_literals;

namespace co_http_uring {

namespace {

constexpr std::chrono::seconds DEFAULT_READ_TIMEOUT = 30s;
constexpr u64 DEFAULT_MAX_REQUEST_PRE_BODY_SIZE = 1024 * 1024;

constexpr i64 SQE_DATA_STOP = -1;
constexpr i64 SQE_DATA_ACCEPT = -2;

constexpr std::string_view INVALID_REQUEST_RESPONSE =
    "HTTP/1.1 400 Bad Request\r\n"
    "Connection: close\r\n"
    "\r\n";

} // namespace

thread_local Server *Server::thread_instance_ = nullptr;

Server::Server(RequestHandler handler, unsigned int sq_entries) :
    read_timeout_(DEFAULT_READ_TIMEOUT),
    max_request_pre_body_size_(DEFAULT_MAX_REQUEST_PRE_BODY_SIZE),
    handler_{std::move(handler)},
    ring_{sq_entries},
    stop_eventfd_{0, EFD_CLOEXEC} {
}

void Server::submit_accept() {
    IoUringSqe sqe = ring_.get_sqe();
    socket_.prep_multishot_accept_direct(sqe);
    sqe.set_data64(SQE_DATA_ACCEPT);
    ring_.submit();
}

Task<> Server::serve_connection(Connection conn) {
    ConnectionReader &reader = conn.reader();
    ConnectionWriter &writer = conn.writer();
    bool keep_alive = true;

    while (keep_alive) {
        reader.set_bytes_remaining(max_request_pre_body_size_);
        std::variant<Request, Error> result = co_await Request::receive(reader);

        if (const auto *error = std::get_if<Error>(&result)) {
            if (*error == Error::INVALID_REQUEST) {
                co_await writer.write(INVALID_REQUEST_RESPONSE);
                co_await writer.flush();
            }

            break;
        }

        const auto &req = std::get<Request>(result);
        reader.set_bytes_remaining(req.content_length());
        keep_alive = req.keep_alive();
        co_await handler_(req, {writer, std::min(req.http_version().minor, 1)});
    }

    co_await conn.close();
}

void Server::handle_stop_cqe(const IoUringCqe &&cqe) {
    i32 res = cqe.res();
    ring_.seen_cqe(cqe);

    if (res < 0) {
        const char *what = "stop CQE failed";
        throw std::system_error(-res, std::generic_category(), what);
    }
}

void Server::handle_accept_cqe(const IoUringCqe &&cqe) {
    i32 res = cqe.res();
    u32 flags = cqe.flags();
    ring_.seen_cqe(cqe);

    if ((flags & IORING_CQE_F_MORE) == 0) {
        submit_accept();
    }

    if (res < 0) {
        fmt::print(stderr, "accept CQE failed: {}\n", std::strerror(-res));
        return;
    }

    files_[res] = res;
    tasks_.emplace(res, serve_connection(Connection(res)));
}

void Server::handle_coroutine_cqe(const IoUringCqe &&cqe) {
    i32 res = cqe.res();

    if (res == -ECANCELED) {
        ring_.seen_cqe(cqe);
        return;
    }

    int client_fixed_fd = static_cast<int>(cqe.get_data64());
    std::coroutine_handle<> coroutine = coroutines_.at(client_fixed_fd);
    coroutine.resume();

    if (!coroutines_.contains(client_fixed_fd)) {
        files_[client_fixed_fd] = -1;
        tasks_.erase(client_fixed_fd);
    }
}

void Server::run(const Ipv4Address &address, int max_pending_conns) {
    thread_instance_ = this;

    socket_.bind(address);
    socket_.listen(max_pending_conns);

    files_.resize(2 * max_pending_conns, -1);
    ring_.register_files(files_);

    IoUringSqe sqe = ring_.get_sqe();
    u64 stop_eventfd_value{};
    stop_eventfd_.prep_read(sqe, &stop_eventfd_value);
    sqe.set_data64(SQE_DATA_STOP);

    submit_accept();

    while (stop_eventfd_value == 0) {
        IoUringCqe cqe = ring_.wait_cqe();
        i64 data = static_cast<i64>(cqe.get_data64());

        switch (data) {
        case SQE_DATA_STOP: handle_stop_cqe(std::move(cqe)); break;
        case SQE_DATA_ACCEPT: handle_accept_cqe(std::move(cqe)); break;
        default: handle_coroutine_cqe(std::move(cqe)); break;
        }
    }

    thread_instance_ = nullptr;
}

void Server::stop() {
    stop_eventfd_.write(Eventfd::MAX_VALUE);
}

} // namespace co_http_uring
