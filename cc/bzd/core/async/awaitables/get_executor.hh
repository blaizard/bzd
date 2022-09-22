#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to get the current executor.
struct GetExecutor : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		executor_ = &caller.promise().getExecutor();
		return false;
	}
	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr auto& await_resume() noexcept { return *executor_; }
	Executor* executor_{nullptr};
};

} // namespace bzd::coroutine::impl
