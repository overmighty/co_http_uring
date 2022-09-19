// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/connection_reader.hpp"
#include "co_http_uring/error.hpp"
#include "co_http_uring/headers.hpp"
#include "co_http_uring/request.hpp"
#include "co_http_uring/response_writer.hpp"
#include "co_http_uring/server.hpp"
#include "co_http_uring/socket_address.hpp"
#include "co_http_uring/status_code.hpp"
#include "co_http_uring/task.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <coroutine>
#include <string_view>
#include <thread>
#include <variant>
#include <vector>

extern "C" {
#include <netinet/in.h>
}

using namespace std::literals::chrono_literals;

namespace co_http_uring {

namespace {

constexpr u16 TEST_PORT = 8000;

} // namespace

Task<> handle_request(const Request &req, ResponseWriter res) {
    co_await res.write_status(StatusCode::OK);

    std::variant<std::string_view, Error> result = co_await req.body().read();
    std::vector<char> buffer;

    while (const auto *data = std::get_if<std::string_view>(&result)) {
        auto end_diff = buffer.cend() - buffer.cbegin();
        buffer.resize(buffer.size() + data->size());
        std::copy(data->cbegin(), data->cend(), buffer.begin() + end_diff);

        result = co_await req.body().read();
    }

    co_await res.write_body({buffer.begin(), buffer.end()});
    co_await res.send();

    if (req.contains_header("x-test-stop")) {
        auto *server = Server::thread_instance();
        std::jthread stopper{[server] {
            std::this_thread::sleep_for(3s);
            server->stop();
        }};
        stopper.detach();
    }
}

TEST(ServerTest, ManualTest) {
    int max_pending_conns = 2;
    Server server{
        handle_request,
        static_cast<unsigned int>(4 * max_pending_conns + 2),
    };
    server.run({INADDR_ANY, TEST_PORT}, max_pending_conns);
}

} // namespace co_http_uring
