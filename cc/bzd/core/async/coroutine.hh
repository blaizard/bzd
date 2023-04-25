#pragma once

#if __has_include(<coroutine>)

#include <coroutine>

namespace bzd::async::impl {
template <class promise_type = void>
using coroutine_handle = ::std::coroutine_handle<promise_type>;
using suspend_always = ::std::suspend_always;
using suspend_never = ::std::suspend_never;
// NOLINTNEXTLINE(readability-identifier-naming)
inline auto noop_coroutine() noexcept { return ::std::noop_coroutine(); }
} // namespace bzd::async::impl

#elif __has_include(<experimental/coroutine>)

#include <experimental/coroutine>

namespace bzd::async::impl {
template <class promise_type = void>
using coroutine_handle = ::std::experimental::coroutine_handle<promise_type>;
using suspend_always = ::std::experimental::suspend_always;
using suspend_never = ::std::experimental::suspend_never;
// NOLINTNEXTLINE(readability-identifier-naming)
inline auto noop_coroutine() noexcept { return ::std::experimental::noop_coroutine(); }
} // namespace bzd::async::impl

#else
static_assert(false, "Compiler is missing support for couroutines.");
#endif
