// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_TCP_SOCKET_HPP
#define CO_HTTP_URING_TCP_SOCKET_HPP

#include "io_uring.hpp"
#include "socket_address.hpp"

namespace co_http_uring {

class TcpSocket {
    int fd_;

public:
    TcpSocket();

    ~TcpSocket();

    TcpSocket(const TcpSocket &) = delete;
    TcpSocket &operator=(const TcpSocket &) = delete;

    TcpSocket(TcpSocket &&) = default;
    TcpSocket &operator=(TcpSocket &&) = default;

    void bind(const Ipv4Address &addr) const;

    void listen(int max_pending_conns) const;

    void prep_multishot_accept_direct(IoUringSqe &sqe) const {
        sqe.prep_multishot_accept_direct(fd_, nullptr, nullptr, 0);
    }
};

} // namespace co_http_uring

#endif
