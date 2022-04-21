#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to yield the current execution.
struct Yield : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		caller.promise().schedule();
		return true;
	}
};

} // namespace bzd::coroutine::impl
