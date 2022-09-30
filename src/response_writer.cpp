// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/response_writer.hpp"

#include "co_http_uring/connection_writer.hpp"
#include "co_http_uring/error.hpp"
#include "co_http_uring/headers.hpp"
#include "co_http_uring/http_utils.hpp"
#include "co_http_uring/status_code.hpp"
#include "co_http_uring/task.hpp"

#include <coroutine>
#include <string>

namespace co_http_uring {

ResponseWriter::ResponseWriter(
    ConnectionWriter &writer,
    int http_minor_version
) :
    writer_{&writer},
    http_minor_version_{http_minor_version},
    state_{State::STATUS_LINE} {
}

Task<std::optional<Error>> ResponseWriter::write_status(StatusCode code) {
    if (state_ != State::STATUS_LINE) {
        co_return Error::UNEXPECTED_RESPONSE_STATE;
    }

    std::string base_status_line{http_utils::HTTP_1_VERSION_PREFIX};
    base_status_line += static_cast<char>(http_minor_version_ + '0');
    base_status_line += " ";
    base_status_line += std::to_string(static_cast<int>(code));
    base_status_line += " ";

    std::optional<Error> error = co_await writer_->write(base_status_line);

    if (error ||
        (error = co_await writer_->write_line(status_code_message(code)))) {
        co_return error;
    }

    state_ = State::HEADERS;
    co_return {};
}

Task<std::optional<Error>>
ResponseWriter::write_header(std::string_view name, std::string_view value) {
    if (state_ != State::HEADERS) {
        co_return Error::UNEXPECTED_RESPONSE_STATE;
    }

    std::optional<Error> error = co_await writer_->write(name);

    if (error || (error = co_await writer_->write(": "))) {
        co_return error;
    }

    co_return co_await writer_->write_line(value);
}

Task<std::optional<Error>> ResponseWriter::write_body(std::string_view body) {
    if (state_ != State::HEADERS) {
        co_return Error::UNEXPECTED_RESPONSE_STATE;
    }

    std::optional<Error> error = co_await write_header(
        headers::CONTENT_LENGTH,
        std::to_string(body.size())
    );

    if (error || (error = co_await writer_->write_line()) ||
        (error = co_await writer_->write(body))) {
        co_return *error;
    }

    state_ = State::STATUS_LINE;
    co_return {};
}

Task<std::optional<Error>> ResponseWriter::send() {
    co_return co_await writer_->flush();
}

} // namespace co_http_uring
