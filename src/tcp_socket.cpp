// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/tcp_socket.hpp"

#include "co_http_uring/socket_address.hpp"

#include <cerrno>
#include <system_error>

extern "C" {
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
}

namespace co_http_uring {

TcpSocket::TcpSocket() : fd_(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
    if (fd_ == -1) {
        const char *what = "socket() failed";
        throw std::system_error(errno, std::generic_category(), what);
    }
}

TcpSocket::~TcpSocket() {
    ::close(fd_);
}

void TcpSocket::bind(const Ipv4Address &addr) const {
    int reuse_port = 1;
    int ret = ::setsockopt(
        fd_,
        SOL_SOCKET,
        SO_REUSEPORT,
        &reuse_port,
        sizeof(reuse_port)
    );

    if (ret == -1) {
        const char *what = "setsockopt() failed";
        throw std::system_error(errno, std::generic_category(), what);
    }

    if (::bind(fd_, addr.sockaddr(), addr.sockaddr_size()) == -1) {
        const char *what = "bind() failed";
        throw std::system_error(errno, std::generic_category(), what);
    }
}

void TcpSocket::listen(int max_pending_conns) const {
    if (::listen(fd_, max_pending_conns) == -1) {
        const char *what = "listen() failed";
        throw std::system_error(errno, std::generic_category(), what);
    }
}

} // namespace co_http_uring
