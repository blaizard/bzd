#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::async::awaitable {

/// Awaitable to get the current executor.
struct GetExecutor : public bzd::async::impl::suspend_always
{
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::async::impl::coroutine_handle<T> caller) noexcept
	{
		executor_ = &caller.promise().getExecutor();
		return false;
	}
	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr auto& await_resume() noexcept { return *executor_; }
	bzd::async::impl::Executor<bzd::async::impl::PromiseBase>* executor_{nullptr};
};

} // namespace bzd::async::awaitable
