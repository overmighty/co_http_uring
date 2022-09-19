// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_SERVER_HPP
#define CO_HTTP_URING_SERVER_HPP

#include "eventfd.hpp"
#include "io_uring.hpp"
#include "socket_address.hpp"
#include "task.hpp"
#include "tcp_socket.hpp"
#include "types.hpp"

#include <chrono>
#include <coroutine>
#include <functional>
#include <map>
#include <vector>

namespace co_http_uring {

class Connection;
class Request;
class ResponseWriter;

class Server {
    using RequestHandler =
        std::function<Task<>(const Request &, ResponseWriter)>;

    static thread_local Server *thread_instance_;

    std::chrono::seconds read_timeout_;
    u64 max_request_pre_body_size_;
    TcpSocket socket_;
    RequestHandler handler_;
    IoUring ring_;
    Eventfd stop_eventfd_;
    std::vector<int> files_;
    std::map<int, Task<>> tasks_;
    std::map<int, std::coroutine_handle<>> coroutines_;

    void submit_accept();

    Task<> serve_connection(Connection conn);

    void handle_stop_cqe(const IoUringCqe &&cqe);

    void handle_accept_cqe(const IoUringCqe &&cqe);

    void handle_coroutine_cqe(const IoUringCqe &&cqe);

public:
    explicit Server(RequestHandler handler, unsigned int sq_entries);

    ~Server() = default;

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    Server(Server &&) = default;
    Server &operator=(Server &&) = default;

    static Server *thread_instance() { return thread_instance_; }

    [[nodiscard]] std::chrono::seconds read_timeout() const {
        return read_timeout_;
    }

    void set_read_timeout(std::chrono::seconds read_timeout) {
        read_timeout_ = read_timeout;
    }

    [[nodiscard]] u64 max_request_pre_body_size() const {
        return max_request_pre_body_size_;
    }

    void set_max_request_pre_body_size(u64 max_request_pre_body_size) {
        max_request_pre_body_size_ = max_request_pre_body_size;
    }

    IoUring &ring() { return ring_; }

    std::map<int, std::coroutine_handle<>> &coroutines() { return coroutines_; }

    void run(const Ipv4Address &address, int max_pending_conns);

    void stop();
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_SERVER_HPP
