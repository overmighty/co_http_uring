// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_IO_URING_HPP
#define CO_HTTP_URING_IO_URING_HPP

#include "types.hpp"

#include <liburing.h>

#include <cstddef>
#include <vector>

extern "C" {
#include <linux/time_types.h>
#include <sys/socket.h>
};

namespace co_http_uring {

class IoUringSqe {
    io_uring_sqe *sqe_;

public:
    explicit IoUringSqe(io_uring_sqe *sqe) : sqe_(sqe) {}

    ~IoUringSqe() = default;

    IoUringSqe(const IoUringSqe &) = delete;
    IoUringSqe &operator=(const IoUringSqe &) = delete;

    IoUringSqe(IoUringSqe &&) noexcept = default;
    IoUringSqe &operator=(IoUringSqe &&) noexcept = default;

    void set_data64(u64 data) { io_uring_sqe_set_data64(sqe_, data); }

    void set_flags(unsigned int flags) { io_uring_sqe_set_flags(sqe_, flags); }

    void prep_multishot_accept_direct(
        int fd,
        sockaddr *addr,
        socklen_t *addrlen,
        int flags
    ) {
        io_uring_prep_multishot_accept_direct(sqe_, fd, addr, addrlen, flags);
    }

    void prep_link_timeout(__kernel_timespec *ts) {
        io_uring_prep_link_timeout(sqe_, ts, 0);
    }

    void prep_close_direct(unsigned int file_index) {
        io_uring_prep_close_direct(sqe_, file_index);
    }

    void prep_read(int fd, void *buf, unsigned int nbytes, u64 offset) {
        io_uring_prep_read(sqe_, fd, buf, nbytes, offset);
    }

    void prep_write(int fd, void *buffer, unsigned int nbytes, u64 offset) {
        io_uring_prep_write(sqe_, fd, buffer, nbytes, offset);
    }

    void prep_send(int sockfd, void *buf, std::size_t len, int flags) {
        io_uring_prep_send(sqe_, sockfd, buf, len, flags);
    }

    void prep_recv(int sockfd, void *buf, std::size_t len, int flags) {
        io_uring_prep_recv(sqe_, sockfd, buf, len, flags);
    }
};

class IoUringCqe {
    friend class IoUring;

    io_uring_cqe *cqe_;

    [[nodiscard]] io_uring_cqe *cqe() const { return cqe_; }

public:
    explicit IoUringCqe(io_uring_cqe *cqe) : cqe_(cqe) {}

    ~IoUringCqe() = default;

    IoUringCqe(const IoUringCqe &) = delete;
    IoUringCqe &operator=(const IoUringCqe &) = delete;

    IoUringCqe(IoUringCqe &&) noexcept = default;
    IoUringCqe &operator=(IoUringCqe &&) noexcept = default;

    [[nodiscard]] i32 res() const { return cqe_->res; }

    [[nodiscard]] u32 flags() const { return cqe_->flags; }

    [[nodiscard]] u64 get_data64() const {
        return io_uring_cqe_get_data64(cqe_);
    }
};

class IoUring {
    io_uring ring_{};

public:
    explicit IoUring(unsigned int entries);

    ~IoUring();

    IoUring(const IoUring &) = delete;
    IoUring &operator=(const IoUring &) = delete;

    IoUring(IoUring &&) = default;
    IoUring &operator=(IoUring &&) = default;

    void register_files(const std::vector<int> &files);

    IoUringSqe get_sqe();

    void submit();

    IoUringCqe peek_cqe();

    IoUringCqe wait_cqe();

    void seen_cqe(const IoUringCqe &cqe) {
        io_uring_cqe_seen(&ring_, cqe.cqe());
    }
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_IO_URING_HPP
