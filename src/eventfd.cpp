// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/eventfd.hpp"

#include "co_http_uring/types.hpp"

#include <cerrno>
#include <system_error>

extern "C" {
#include <sys/eventfd.h>
#include <unistd.h>
}

namespace co_http_uring {

Eventfd::Eventfd(int initial_value, int flags) :
    fd_{::eventfd(initial_value, flags)} {
    if (fd_ == -1) {
        const char *what = "eventfd() failed";
        throw std::system_error(errno, std::generic_category(), what);
    }
}

Eventfd::~Eventfd() {
    ::close(fd_);
}

void Eventfd::write(u64 value) const {
    if (::write(fd_, &value, sizeof(value)) == -1) {
        const char *what = "write() failed";
        throw std::system_error(errno, std::generic_category(), what);
    }
}

} // namespace co_http_uring
