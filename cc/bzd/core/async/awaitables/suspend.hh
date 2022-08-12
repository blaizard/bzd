#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to detach an async object and optionally execute a callable.
template <class OnSuspend, class OnCancellation>
class Suspend : public bzd::coroutine::impl::suspend_always
{
private: // Traits.
	using Self = Suspend<OnSuspend, OnCancellation>;

public: // Constructor.
	constexpr Suspend(OnSuspend&& onSuspend, OnCancellation&& onCancellation) noexcept :
		onSuspend_{bzd::move(onSuspend)}, onCancellation_{bzd::move(onCancellation)}
	{
	}
	constexpr Suspend(OnSuspend&& onSuspend) noexcept : Suspend{bzd::move(onSuspend), []() {}} {}

public: // Coroutine specializations.
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		bzd::coroutine::impl::Executable& promise = caller.promise();
		promise.suspend();
		onSuspend_(promise);
		return true;
	}

private:
	OnSuspend onSuspend_;
	OnCancellation onCancellation_;
};

template <class OnSuspend>
Suspend(OnSuspend&& onSuspend) -> Suspend<OnSuspend, void (*)(void)>;

} // namespace bzd::coroutine::impl
