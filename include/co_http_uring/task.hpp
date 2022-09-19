// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#ifndef CO_HTTP_URING_TASK_HPP
#define CO_HTTP_URING_TASK_HPP

#include <fmt/core.h>

#include <coroutine>
#include <cstdio>
#include <utility>

namespace co_http_uring {

template <typename T = void>
class Task {
public:
    class promise_type;

private:
    using Coroutine = std::coroutine_handle<promise_type>;

    Coroutine coroutine_;

    struct ParentTaskAwaitable {
        [[nodiscard]] bool await_ready() const noexcept { return false; }

        std::coroutine_handle<> await_suspend(Coroutine coroutine
        ) const noexcept {
            if (!coroutine.promise().parent_coroutine_) {
                return std::noop_coroutine();
            }

            return coroutine.promise().parent_coroutine_;
        }

        void await_resume() const noexcept {}
    };

public:
    class promise_type {
        friend Task;
        friend ParentTaskAwaitable;

        std::coroutine_handle<> parent_coroutine_;
        T return_value_;

    public:
        Task get_return_object() {
            return Task(Coroutine::from_promise(*this));
        }

        std::suspend_never initial_suspend() { return {}; }

        ParentTaskAwaitable final_suspend() noexcept { return {}; }

        void return_value(T &&value) { return_value_ = value; }

        void unhandled_exception() {
            fmt::print(stderr, "unhandled exception in Task\n");
        }
    };

    explicit Task(Coroutine coroutine) : coroutine_{coroutine} {}

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    Task(Task &&) noexcept = default;
    Task &operator=(Task &&) noexcept = default;

    ~Task() {
        if (coroutine_.done()) {
            coroutine_.destroy();
        }
    }

    [[nodiscard]] bool await_ready() const { return coroutine_.done(); }

    void await_suspend(std::coroutine_handle<> coroutine) const {
        coroutine_.promise().parent_coroutine_ = coroutine;
    }

    T await_resume() const {
        return std::move(coroutine_.promise().return_value_);
    }
};

template <>
class Task<void> {
public:
    class promise_type;

private:
    using Coroutine = std::coroutine_handle<promise_type>;

    Coroutine coroutine_;

    struct ParentTaskAwaitable {
        [[nodiscard]] bool await_ready() const noexcept { return false; }

        [[nodiscard]] std::coroutine_handle<> await_suspend(Coroutine coroutine
        ) const noexcept {
            if (!coroutine.promise().parent_coroutine_) {
                return std::noop_coroutine();
            }

            return coroutine.promise().parent_coroutine_;
        }

        void await_resume() const noexcept {}
    };

public:
    class promise_type {
        friend Task;
        friend ParentTaskAwaitable;

        std::coroutine_handle<> parent_coroutine_;

    public:
        Task get_return_object() {
            return Task(Coroutine::from_promise(*this));
        }

        std::suspend_never initial_suspend() { return {}; }

        ParentTaskAwaitable final_suspend() noexcept { return {}; }

        void return_void() {}

        void unhandled_exception() {
            fmt::print(stderr, "unhandled exception in Task\n");
        }
    };

    explicit Task(Coroutine coroutine) : coroutine_{coroutine} {}

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    Task(Task &&) noexcept = default;
    Task &operator=(Task &&) noexcept = default;

    ~Task() {
        if (coroutine_.done()) {
            coroutine_.destroy();
        }
    }

    [[nodiscard]] bool await_ready() const { return coroutine_.done(); }

    void await_suspend(std::coroutine_handle<> coroutine) const {
        coroutine_.promise().parent_coroutine_ = coroutine;
    }

    void await_resume() const {}
};

} // namespace co_http_uring

#endif // CO_HTTP_URING_TASK_HPP
