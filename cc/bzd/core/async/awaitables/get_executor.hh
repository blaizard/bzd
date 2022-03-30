#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to get the current executor.
struct GetExecutor : public bzd::coroutine::impl::suspend_never
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		executor_ = &caller.promise().getExecutor();
		return false;
	}
	constexpr auto await_resume() noexcept { return executor_; }
	Executor* executor_;
};

} // namespace bzd::coroutine::impl
