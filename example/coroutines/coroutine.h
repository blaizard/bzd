#pragma once

#if __has_include(<coroutine>)

#include <coroutine>

namespace bzd::coroutine::impl
{
template <class promise_type = void>
using coroutine_handle = ::std::coroutine_handle<promise_type>;
using suspend_always = ::std::suspend_always;
inline auto noop_coroutine() noexcept
{
  return ::std::noop_coroutine();
}
}

#elif __has_include(<experimental/coroutine>)

#include <experimental/coroutine>

namespace bzd::coroutine::impl
{
template <class promise_type = void>
using coroutine_handle = ::std::experimental::coroutine_handle<promise_type>;
using suspend_always = ::std::experimental::suspend_always;
inline auto noop_coroutine() noexcept
{
  return ::std::experimental::noop_coroutine();
}
}

#else
static_assert(false, "Compiler is missing support for couroutines.");
#endif
