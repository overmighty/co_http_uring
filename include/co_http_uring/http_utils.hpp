// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_HTTP_UTILS_HPP
#define CO_HTTP_URING_HTTP_UTILS_HPP

#include <string>
#include <string_view>

namespace co_http_uring::http_utils {

constexpr std::string_view HTTP_1_VERSION_PREFIX = "HTTP/1.";

bool is_token(std::string_view str);

bool is_digit(char c);

bool is_number(std::string_view str);

bool is_field_value(std::string_view str);

std::string make_header_name(std::string_view name);

std::string make_header_value(std::string_view value);

} // namespace co_http_uring::http_utils

#endif // CO_HTTP_URING_HTTP_UTILS_HPP
