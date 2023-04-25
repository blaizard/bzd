#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::async::awaitable {

/// Awaitable to get the current executable.
struct GetExecutable : public bzd::async::impl::suspend_always
{
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::async::impl::coroutine_handle<T> caller) noexcept
	{
		promise_ = &caller.promise();
		return false;
	}
	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr auto await_resume() noexcept { return promise_; }
	bzd::async::impl::PromiseBase* promise_;
};

} // namespace bzd::async::awaitable
