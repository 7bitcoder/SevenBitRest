#pragma once

#include <experimental/coroutine>
#include <optional>

namespace sd
{
    template <typename T = void> struct Task
    {
        template <typename U> struct task_promise;

        using promise_type = task_promise<T>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;

        mutable handle_type _handle;

        Task(handle_type handle) : _handle(handle) {}

        Task(Task &&other) noexcept : _handle(other._handle) { other._handle = nullptr; };

        bool await_ready() { return _handle.done(); }

        bool await_suspend(std::experimental::coroutine_handle<> handle)
        {
            if (!_handle.done())
            {
                _handle.resume();
            }

            return false;
        }

        auto await_resume() { return result(); }

        T result() const
        {
            if (!_handle.done())
                _handle.resume();

            if (_handle.promise().m_exception)
                std::rethrow_exception(_handle.promise().m_exception);

            return *_handle.promise().m_value;
        }

        ~Task()
        {
            if (_handle)
                _handle.destroy();
        }

        template <typename U> struct task_promise
        {
            std::optional<T> _value{};
            std::exception_ptr _exception = nullptr;

            auto initial_suspend() { return std::experimental::suspend_always{}; }

            auto final_suspend() { return std::experimental::suspend_always{}; }

            auto return_value(T t)
            {
                _value = t;
                return std::experimental::suspend_always{};
            }

            Task<T> get_return_object() { return {handle_type::from_promise(*this)}; }

            void unhandled_exception() { _exception = std::current_exception(); }

            void rethrow_if_unhandled_exception()
            {
                if (_exception)
                    std::rethrow_exception(std::move(_exception));
            }
        };
    };
} // namespace sd