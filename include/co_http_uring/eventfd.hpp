// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_EVENTFD_HPP
#define CO_HTTP_URING_EVENTFD_HPP

#include "io_uring.hpp"
#include "types.hpp"

#include <limits>

namespace co_http_uring {

class Eventfd {
    int fd_;

public:
    static constexpr u64 MAX_VALUE = std::numeric_limits<u64>::max() - 1;

    Eventfd(int initial_value, int flags);

    ~Eventfd();

    Eventfd(const Eventfd &) = default;
    Eventfd &operator=(const Eventfd &) = default;

    Eventfd(Eventfd &&) = default;
    Eventfd &operator=(Eventfd &&) = default;

    void prep_read(IoUringSqe &sqe, u64 *dst) const {
        sqe.prep_read(fd_, dst, sizeof(*dst), 0);
    }

    void write(u64 value) const;
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_EVENTFD_HPP
