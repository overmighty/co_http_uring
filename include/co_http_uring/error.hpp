// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_ERROR_HPP
#define CO_HTTP_URING_ERROR_HPP

namespace co_http_uring {

enum class Error {
    READ_LIMIT_REACHED,
    BUFFER_FULL,
    READ_ERROR,
    CONNECTION_TIMED_OUT,
    INVALID_REQUEST,
    WRITE_ERROR,
    UNEXPECTED_RESPONSE_STATE,
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_ERROR_HPP
