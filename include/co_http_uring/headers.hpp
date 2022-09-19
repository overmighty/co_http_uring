// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_HEADERS_HPP
#define CO_HTTP_URING_HEADERS_HPP

#include <string_view>

namespace co_http_uring::headers {

constexpr std::string_view CONNECTION = "connection";
constexpr std::string_view CONTENT_LENGTH = "content-length";
constexpr std::string_view HOST = "host";

} // namespace co_http_uring::headers

#endif // CO_HTTP_URING_HEADERS_HPP
