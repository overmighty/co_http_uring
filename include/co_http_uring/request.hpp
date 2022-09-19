// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_REQUEST_HPP
#define CO_HTTP_URING_REQUEST_HPP

#include "connection_reader.hpp"
#include "error.hpp"
#include "http_utils.hpp"
#include "task.hpp"
#include "types.hpp"

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace co_http_uring {

struct HttpVersion {
    int major;
    int minor;
};

class Request {
    using HeaderMap = std::unordered_map<std::string, std::vector<std::string>>;

    std::string method_;
    std::string target_;
    HttpVersion http_version_{};
    HeaderMap headers_;
    u64 content_length_{};
    ConnectionReader *body_{};
    bool keep_alive_{};

    void add_header(std::string_view name, std::string_view value);

    Task<std::optional<Error>> read_request_line(ConnectionReader &reader);

    bool parse_header_line(std::string_view line);

    Task<std::optional<Error>> read_headers(ConnectionReader &reader);

    bool parse_content_length();

public:
    Request() = default;

    static Task<std::variant<Request, Error>> receive(ConnectionReader &reader);

    [[nodiscard]] std::string_view method() const { return method_; }

    [[nodiscard]] std::string_view target() const { return target_; }

    [[nodiscard]] HttpVersion http_version() const { return http_version_; }

    [[nodiscard]] const HeaderMap &headers() const { return headers_; }

    u64 content_length() const { return content_length_; }

    ConnectionReader &body() const { return *body_; }

    bool keep_alive() const { return keep_alive_; }

    bool contains_header(std::string_view name) const {
        return headers_.contains(http_utils::make_header_name(name));
    }

    std::optional<std::reference_wrapper<const std::vector<std::string>>>
    get_header(std::string_view name) const {
        auto it = headers_.find(http_utils::make_header_name(name));

        if (it == headers_.cend()) {
            return {};
        }

        return it->second;
    }
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_REQUEST_HPP
