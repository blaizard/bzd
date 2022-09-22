#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to get the current executable.
struct GetExecutable : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		executable_ = &caller.promise();
		return false;
	}
	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr auto await_resume() noexcept { return executable_; }
	Executable* executable_;
};

} // namespace bzd::coroutine::impl
