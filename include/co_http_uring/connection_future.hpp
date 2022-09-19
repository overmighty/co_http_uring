// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_CONNECTION_FUTURE_HPP
#define CO_HTTP_URING_CONNECTION_FUTURE_HPP

#include "server.hpp"

#include <fmt/core.h>

namespace co_http_uring {

template <typename T>
struct ConnectionWrapperTraits {
    static int fixed_fd(const T *wrapper) { return wrapper->fixed_fd(); }
};

template <typename ConnectionWrapper>
class ConnectionFuture {
    using ConnectionTraits = ConnectionWrapperTraits<ConnectionWrapper>;

    ConnectionWrapper *wrapper_;

public:
    explicit ConnectionFuture(ConnectionWrapper *wrapper) : wrapper_{wrapper} {}

    [[nodiscard]] bool await_ready() const { return false; }

    void await_suspend(std::coroutine_handle<> coroutine) const {
        auto [it, inserted] = Server::thread_instance()->coroutines().emplace(
            ConnectionTraits::fixed_fd(wrapper_),
            coroutine
        );

        if (!inserted) {
            fmt::print(stderr, "consecutive await_suspend()\n");
        }
    }

    void await_resume() const {
        Server::thread_instance()->coroutines().erase(
            ConnectionTraits::fixed_fd(wrapper_)
        );
    }
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_CONNECTION_FUTURE_HPP
