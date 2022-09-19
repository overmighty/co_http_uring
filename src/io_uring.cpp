// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/io_uring.hpp"

#include <liburing.h>

#include <stdexcept>
#include <system_error>

namespace co_http_uring {

IoUring::IoUring(unsigned int entries) {
    int ret = io_uring_queue_init(entries, &ring_, 0);

    if (ret < 0) {
        const char *what = "io_uring_queue_init() failed";
        throw std::system_error(-ret, std::generic_category(), what);
    }
}

IoUring::~IoUring() {
    io_uring_queue_exit(&ring_);
}

void IoUring::register_files(const std::vector<int> &files) {
    int ret = io_uring_register_files(&ring_, files.data(), files.size());

    if (ret < 0) {
        const char *what = "io_uring_register_files() failed";
        throw std::system_error(-ret, std::generic_category(), what);
    }
}

IoUringSqe IoUring::get_sqe() {
    io_uring_sqe *sqe = io_uring_get_sqe(&ring_);

    if (sqe == nullptr) {
        throw std::runtime_error("io_uring_get_sqe() returned nullptr");
    }

    return IoUringSqe(sqe);
}

void IoUring::submit() {
    int ret = io_uring_submit(&ring_);

    if (ret < 0) {
        const char *what = "io_uring_submit() failed";
        throw std::system_error(-ret, std::generic_category(), what);
    }
}

IoUringCqe IoUring::peek_cqe() {
    io_uring_cqe *cqe;
    int ret = io_uring_peek_cqe(&ring_, &cqe);

    if (ret < 0) {
        const char *what = "io_uring_wait_cqe() failed";
        throw std::system_error(-ret, std::generic_category(), what);
    }

    return IoUringCqe(cqe);
}

IoUringCqe IoUring::wait_cqe() {
    io_uring_cqe *cqe;
    int ret = io_uring_wait_cqe(&ring_, &cqe);

    if (ret < 0) {
        const char *what = "io_uring_wait_cqe() failed";
        throw std::system_error(-ret, std::generic_category(), what);
    }

    return IoUringCqe(cqe);
}

} // namespace co_http_uring
