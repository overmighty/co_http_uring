// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/connection_reader.hpp"

#include "co_http_uring/connection_future.hpp"
#include "co_http_uring/error.hpp"
#include "co_http_uring/io_uring.hpp"
#include "co_http_uring/server.hpp"
#include "co_http_uring/task.hpp"
#include "co_http_uring/types.hpp"

#include <fmt/core.h>
#include <liburing.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <coroutine>
#include <cstdio>
#include <cstring>
#include <limits>
#include <optional>

extern "C" {
#include <linux/time_types.h>
}

namespace co_http_uring {

ConnectionReader::ConnectionReader(int fixed_fd) :
    fixed_fd_{fixed_fd},
    bytes_remaining_{std::numeric_limits<i64>::max()},
    buffer_{std::make_unique<Buffer>()},
    begin_{buffer_->cbegin()},
    end_{begin_} {
}

ConnectionFuture<ConnectionReader> ConnectionReader::submit_recv() {
    if (begin_ == end_) {
        begin_ = buffer_->cbegin();
        end_ = begin_;
    }

    auto *server = Server::thread_instance();
    IoUring &ring = server->ring();

    IoUringSqe sqe = ring.get_sqe();
    unsigned int num_bytes =
        std::min(static_cast<u64>(buffer_->cend() - end_), bytes_remaining_);
    u64 offset = end_ - buffer_->cbegin();
    sqe.prep_recv(fixed_fd_, buffer_->data() + offset, num_bytes, 0);
    sqe.set_data64(fixed_fd_);
    sqe.set_flags(IOSQE_FIXED_FILE | IOSQE_IO_LINK);

    sqe = ring.get_sqe();
    __kernel_timespec timeout{
        .tv_sec = server->read_timeout().count(),
        .tv_nsec = 0,
    };
    sqe.prep_link_timeout(&timeout);
    sqe.set_data64(fixed_fd_);

    ring.submit();
    return ConnectionFuture<ConnectionReader>(this);
}

Task<std::optional<Error>> ConnectionReader::fill() {
    if (bytes_remaining_ == 0) {
        co_return Error::READ_LIMIT_REACHED;
    }

    if (end_ == buffer_->cend()) {
        co_return Error::BUFFER_FULL;
    }

    co_await submit_recv();

    IoUring &ring = Server::thread_instance()->ring();

    IoUringCqe cqe = ring.peek_cqe();
    i32 res = cqe.res();
    ring.seen_cqe(cqe);

    if (res == 0) {
        co_return Error::CONNECTION_CLOSED;
    }

    if (res < 0) {
        if (res != -ETIME) {
            fmt::print(stderr, "read error: {}\n", std::strerror(-res));
            co_return Error::READ_ERROR;
        }

        co_return Error::CONNECTION_TIMED_OUT;
    }

    bytes_remaining_ -= res;
    end_ += res;
    co_return {};
}

Task<std::variant<std::string_view, Error>> ConnectionReader::read() {
    if (bytes_remaining_ == 0) {
        co_return Error::READ_LIMIT_REACHED;
    }

    while (end_ != buffer_->cend() && bytes_remaining_ > 0) {
        if (std::optional<Error> error = co_await fill()) {
            co_return *error;
        }
    }

    auto begin = begin_;
    begin_ = end_;
    co_return std::string_view{begin, end_};
}

Task<std::variant<std::string_view, Error>> ConnectionReader::read_line() {
    auto line_end = std::find(begin_, end_, '\n');

    while (line_end == end_) {
        if (std::optional<Error> error = co_await fill()) {
            co_return *error;
        }

        line_end = std::find(begin_, end_, '\n');
    }

    auto line_begin = begin_;
    begin_ = line_end + 1;
    std::string_view line{line_begin, line_end};

    if (line.ends_with('\r')) {
        line.remove_suffix(1);
    }

    co_return line;
}

} // namespace co_http_uring
