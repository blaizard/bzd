#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::async::awaitable {

/// Awaitable to yield the current execution.
struct Yield : public bzd::async::impl::suspend_always
{
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::async::impl::coroutine_handle<T> caller) noexcept
	{
		auto& executable{caller.promise()};
		executable.reschedule();
		return true;
	}
};

} // namespace bzd::async::awaitable
