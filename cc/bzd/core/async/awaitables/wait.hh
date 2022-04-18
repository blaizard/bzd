#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to detach an async object and optionally execute a callable.
template <class Callable>
class Wait : public bzd::coroutine::impl::suspend_always
{
public: // Constructor.
	constexpr Wait(Callable&& callback) noexcept : callback_{bzd::move(callback)} {}

public: // Coroutine specializations.
	template <class T>
	constexpr void await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		bzd::coroutine::impl::Executable& promise = caller.promise();
		callback_(promise);
	}

private:
	Callable callback_;
};

} // namespace bzd::coroutine::impl
