// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/connection.hpp"

#include "co_http_uring/connection_future.hpp"
#include "co_http_uring/io_uring.hpp"
#include "co_http_uring/server.hpp"
#include "co_http_uring/task.hpp"
#include "co_http_uring/types.hpp"

#include <fmt/core.h>

#include <coroutine>
#include <cstdio>
#include <cstring>

namespace co_http_uring {

Connection::Connection(int fixed_fd) :
    fixed_fd_{fixed_fd},
    reader_{fixed_fd_},
    writer_{fixed_fd_} {
}

ConnectionFuture<Connection> Connection::submit_close() {
    IoUring &ring = Server::thread_instance()->ring();

    IoUringSqe sqe = ring.get_sqe();
    sqe.prep_close_direct(fixed_fd_);
    sqe.set_data64(fixed_fd_);

    ring.submit();
    return ConnectionFuture<Connection>(this);
}

Task<> Connection::close() {
    co_await submit_close();
    IoUring &ring = Server::thread_instance()->ring();

    IoUringCqe cqe = ring.peek_cqe();
    i32 res = cqe.res();
    ring.seen_cqe(cqe);

    if (res < 0) {
        fmt::print(stderr, "conn close failed: {}\n", std::strerror(-res));
    }
}

} // namespace co_http_uring
