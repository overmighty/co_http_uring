// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/connection_writer.hpp"

#include "co_http_uring/connection_future.hpp"
#include "co_http_uring/error.hpp"
#include "co_http_uring/io_uring.hpp"
#include "co_http_uring/server.hpp"
#include "co_http_uring/task.hpp"
#include "co_http_uring/types.hpp"

#include <fmt/core.h>
#include <liburing.h>

#include <algorithm>
#include <coroutine>
#include <cstdio>
#include <cstring>

namespace co_http_uring {

namespace {

constexpr std::string_view LINE_SEPARATOR = "\r\n";

} // namespace

ConnectionWriter::ConnectionWriter(int fixed_fd) :
    fixed_fd_(fixed_fd),
    buffer_(std::make_unique<Buffer>()),
    begin_(buffer_->begin()),
    end_(begin_) {
}

ConnectionFuture<ConnectionWriter> ConnectionWriter::submit_write() {
    IoUring &ring = Server::thread_instance()->ring();

    IoUringSqe sqe = ring.get_sqe();
    unsigned int num_bytes = end_ - begin_;
    u64 offset = begin_ - buffer_->begin();
    sqe.prep_send(fixed_fd_, buffer_->data() + offset, num_bytes, 0);
    sqe.set_data64(fixed_fd_);
    sqe.set_flags(IOSQE_FIXED_FILE);

    ring.submit();
    return ConnectionFuture<ConnectionWriter>(this);
}

Task<std::optional<Error>> ConnectionWriter::flush() {
    while (begin_ != end_) {
        co_await submit_write();
        IoUring &ring = Server::thread_instance()->ring();

        IoUringCqe cqe = ring.peek_cqe();
        int res = cqe.res();
        ring.seen_cqe(cqe);

        if (res < 0) {
            fmt::print(stderr, "write error: {}\n", std::strerror(-res));
            co_return Error::WRITE_ERROR;
        }

        begin_ += res;
    }

    begin_ = buffer_->begin();
    end_ = begin_;
    co_return {};
}

Task<std::optional<Error>> ConnectionWriter::write(std::string_view str) {
    while (static_cast<std::size_t>(buffer_->end() - end_) < str.size()) {
        std::optional<Error> error = co_await flush();

        if (error) {
            co_return *error;
        }
    }

    end_ = std::copy(str.cbegin(), str.cend(), end_);
    co_return {};
}

Task<std::optional<Error>> ConnectionWriter::write_line(std::string_view line) {
    std::size_t line_real_size = line.size() + LINE_SEPARATOR.size();

    if (static_cast<std::size_t>(buffer_->end() - end_) < line_real_size) {
        if (std::optional<Error> error = co_await flush()) {
            co_return *error;
        }
    }

    auto line_end = std::copy(line.cbegin(), line.cend(), end_);
    end_ = std::copy(LINE_SEPARATOR.cbegin(), LINE_SEPARATOR.cend(), line_end);
    co_return {};
}

} // namespace co_http_uring
