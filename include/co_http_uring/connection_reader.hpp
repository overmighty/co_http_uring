// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_CONNECTION_READER_HPP
#define CO_HTTP_URING_CONNECTION_READER_HPP

#include "connection_future.hpp"
#include "error.hpp"
#include "task.hpp"
#include "types.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <variant>

namespace co_http_uring {

class ConnectionReader {
    static constexpr std::size_t BUFFER_SIZE = 8192;

    using Buffer = std::array<char, BUFFER_SIZE>;

    int fixed_fd_;
    u64 bytes_remaining_;
    std::unique_ptr<Buffer> buffer_;
    Buffer::const_iterator begin_;
    Buffer::const_iterator end_;

    ConnectionFuture<ConnectionReader> submit_recv();

    Task<std::optional<Error>> fill();

public:
    explicit ConnectionReader(int fixed_fd);

    ~ConnectionReader() = default;

    ConnectionReader(const ConnectionReader &) = delete;
    ConnectionReader &operator=(const ConnectionReader &) = delete;

    ConnectionReader(ConnectionReader &&) noexcept = default;
    ConnectionReader &operator=(ConnectionReader &&) noexcept = default;

    [[nodiscard]] int fixed_fd() const { return fixed_fd_; }

    void set_bytes_remaining(u64 bytes_remaining) {
        bytes_remaining_ = bytes_remaining;
    }

    Task<std::variant<std::string_view, Error>> read();

    Task<std::variant<std::string_view, Error>> read_line();
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_CONNECTION_READER_HPP
