// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_RESPONSE_WRITER_HPP
#define CO_HTTP_URING_RESPONSE_WRITER_HPP

#include "connection_writer.hpp"
#include "error.hpp"
#include "status_code.hpp"
#include "task.hpp"

#include <optional>
#include <string_view>

namespace co_http_uring {

class ResponseWriter {
    enum class State {
        STATUS_LINE,
        HEADERS,
    };

    ConnectionWriter *writer_;
    int http_minor_version_;
    State state_;

public:
    ResponseWriter(ConnectionWriter &writer, int http_minor_version);

    Task<std::optional<Error>> write_status(StatusCode code);

    Task<std::optional<Error>>
    write_header(std::string_view name, std::string_view value);

    Task<std::optional<Error>> write_body(std::string_view body);

    Task<std::optional<Error>> send();
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_RESPONSE_WRITER_HPP
