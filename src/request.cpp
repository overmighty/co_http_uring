// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/request.hpp"

#include "co_http_uring/connection_reader.hpp"
#include "co_http_uring/error.hpp"
#include "co_http_uring/headers.hpp"
#include "co_http_uring/http_utils.hpp"
#include "co_http_uring/task.hpp"
#include "co_http_uring/types.hpp"

#include <algorithm>
#include <charconv>
#include <compare>
#include <coroutine>
#include <system_error>

namespace co_http_uring {

namespace {

struct RequestLine {
    std::string_view method;
    std::string_view target;
    std::string_view http_version;
};

std::optional<RequestLine> split_request_line(std::string_view line) {
    auto method_end = std::find(line.cbegin(), line.cend(), ' ');

    if (method_end == line.cend()) {
        return {};
    }

    auto target_end = std::find(method_end + 1, line.cend(), ' ');

    if (target_end == line.cend()) {
        return {};
    }

    return RequestLine{
        .method{line.cbegin(), method_end},
        .target{method_end + 1, target_end},
        .http_version{target_end + 1, line.cend()},
    };
}

} // namespace

void Request::add_header(std::string_view name, std::string_view value) {
    headers_
        .emplace(http_utils::make_header_name(name), std::vector<std::string>())
        .first->second.emplace_back(http_utils::make_header_value(value));
}

Task<std::optional<Error>> Request::read_request_line(ConnectionReader &reader
) {
    std::variant<std::string_view, Error> line = co_await reader.read_line();

    if (const auto *error = std::get_if<Error>(&line)) {
        co_return *error;
    }

    std::optional<RequestLine> opt_request_line =
        split_request_line(std::get<std::string_view>(line));

    if (!opt_request_line) {
        co_return Error::INVALID_REQUEST;
    }

    RequestLine request_line = *opt_request_line;

    if (!http_utils::is_token(request_line.method) ||
        !request_line.http_version.starts_with(http_utils::HTTP_1_VERSION_PREFIX
        )) {
        co_return Error::INVALID_REQUEST;
    }

    request_line.http_version.remove_prefix(
        http_utils::HTTP_1_VERSION_PREFIX.size()
    );

    if (request_line.http_version.size() != 1) {
        co_return Error::INVALID_REQUEST;
    }

    char http_minor_version_char = request_line.http_version[0];

    if (!http_utils::is_digit(http_minor_version_char)) {
        co_return Error::INVALID_REQUEST;
    }

    method_ = request_line.method;
    target_ = request_line.target;
    http_version_ = {1, http_minor_version_char - '0'};
    co_return {};
}

bool Request::parse_header_line(std::string_view line) {
    auto header_name_end = std::find(line.cbegin(), line.cend(), ':');

    if (header_name_end == line.cend()) {
        return false;
    }

    std::string_view header_name{line.cbegin(), header_name_end};
    std::string_view header_value{header_name_end + 1, line.cend()};

    if (!http_utils::is_token(header_name) ||
        !http_utils::is_field_value(header_value)) {
        return false;
    }

    add_header(header_name, {header_name_end + 1, line.cend()});
    return true;
}

Task<std::optional<Error>> Request::read_headers(ConnectionReader &reader) {
    std::variant<std::string_view, Error> result = co_await reader.read_line();

    if (const auto *error = std::get_if<Error>(&result)) {
        co_return *error;
    }

    auto line = std::get<std::string_view>(result);

    while (!line.empty()) {
        if (!parse_header_line(line)) {
            co_return Error::INVALID_REQUEST;
        }

        result = co_await reader.read_line();

        if (std::holds_alternative<Error>(result)) {
            co_return Error::INVALID_REQUEST;
        }

        line = std::get<std::string_view>(result);
    }

    if (http_version_.minor >= 1 && !contains_header(headers::HOST)) {
        co_return Error::INVALID_REQUEST;
    }

    co_return {};
}

bool Request::parse_content_length() {
    auto opt_values = get_header(headers::CONTENT_LENGTH);

    if (!opt_values) {
        return true;
    }

    const std::vector<std::string> &values = *opt_values;
    const std::string &value = values[0];

    if (!http_utils::is_number(value)) {
        return false;
    }

    const char *value_first = value.cbegin().base();
    const char *value_last = value.cend().base();
    auto result = std::from_chars(value_first, value_last, content_length_);

    if (result.ptr != value_last || result.ec != std::errc{0}) {
        return false;
    }

    for (auto it = values.cbegin() + 1; it < values.cend(); it++) {
        if (*it != value) {
            return false;
        }
    }

    return true;
}

Task<std::variant<Request, Error>> Request::receive(ConnectionReader &reader) {
    Request request;
    std::optional<Error> error = co_await request.read_request_line(reader);

    if (error || (error = co_await request.read_headers(reader))) {
        co_return *error;
    }

    if (!request.parse_content_length()) {
        co_return Error::INVALID_REQUEST;
    }

    request.body_ = &reader;
    request.keep_alive_ = request.http_version_.minor >= 1;
    co_return request;
}

} // namespace co_http_uring
