// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_CONNECTION_HPP
#define CO_HTTP_URING_CONNECTION_HPP

#include "connection_future.hpp"
#include "connection_reader.hpp"
#include "connection_writer.hpp"
#include "task.hpp"

namespace co_http_uring {

class Connection {
    int fixed_fd_;
    ConnectionReader reader_;
    ConnectionWriter writer_;

    ConnectionFuture<Connection> submit_close();

public:
    explicit Connection(int fixed_fd);

    [[nodiscard]] int fixed_fd() const { return fixed_fd_; }

    ConnectionReader &reader() { return reader_; }

    ConnectionWriter &writer() { return writer_; }

    Task<> close();
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_CONNECTION_HPP
