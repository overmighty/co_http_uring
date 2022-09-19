// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_CONNECTION_WRITER_HPP
#define CO_HTTP_URING_CONNECTION_WRITER_HPP

#include "connection_future.hpp"
#include "error.hpp"
#include "task.hpp"
#include "types.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>

namespace co_http_uring {

class ConnectionWriter {
    static constexpr std::size_t BUFFER_SIZE = 8192;

    using Buffer = std::array<u8, BUFFER_SIZE>;

    int fixed_fd_;
    std::unique_ptr<Buffer> buffer_;
    Buffer::iterator begin_;
    Buffer::iterator end_;

    ConnectionFuture<ConnectionWriter> submit_write();

public:
    explicit ConnectionWriter(int fixed_fd);

    ~ConnectionWriter() = default;

    ConnectionWriter(const ConnectionWriter &) = delete;
    ConnectionWriter &operator=(const ConnectionWriter &) = delete;

    ConnectionWriter(ConnectionWriter &&) noexcept = default;
    ConnectionWriter &operator=(ConnectionWriter &&) noexcept = default;

    [[nodiscard]] int fixed_fd() const { return fixed_fd_; }

    Task<std::optional<Error>> flush();

    Task<std::optional<Error>> write(std::string_view str);

    Task<std::optional<Error>> write_line(std::string_view line = "");
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_CONNECTION_WRITER_HPP
