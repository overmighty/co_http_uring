// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_SOCKET_ADDRESS_HPP
#define CO_HTTP_URING_SOCKET_ADDRESS_HPP

#include "types.hpp"

extern "C" {
#include <netinet/in.h>
};

namespace co_http_uring {

class Ipv4Address {
    sockaddr_in sockaddr_{};

public:
    Ipv4Address(u32 addr, u16 port) {
        sockaddr_.sin_family = AF_INET;
        sockaddr_.sin_port = ::htons(port);
        sockaddr_.sin_addr = {
            .s_addr = ::htonl(addr),
        };
    }

    [[nodiscard]] const struct sockaddr *sockaddr() const {
        return reinterpret_cast<const struct sockaddr *>(&sockaddr_);
    }

    [[nodiscard]] struct sockaddr *sockaddr() {
        return reinterpret_cast<struct sockaddr *>(&sockaddr_);
    }

    [[nodiscard]] socklen_t sockaddr_size() const { return sizeof(sockaddr_); }

    [[nodiscard]] u16 port() const { return ::ntohs(sockaddr_.sin_port); }

    [[nodiscard]] u32 addr() const {
        return ::ntohl(sockaddr_.sin_addr.s_addr);
    }
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_SOCKET_ADDRESS_HPP
