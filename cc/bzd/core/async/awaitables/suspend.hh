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
		onSuspend_{bzd::move(onSuspend)}, onCancellation_{bzd::move(onCancellation)},
		cancellationObject_{bzd::FunctionRef<void(void)>::toMember<Self, &Self::onCancel>(*this)}
	{
	}
	constexpr Suspend(OnSuspend&& onSuspend) noexcept : Suspend{bzd::move(onSuspend), []() {}} {}

public: // Coroutine specializations.
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		bzd::coroutine::impl::Executable& promise = caller.promise();
		onSuspend_(promise);
		// If there is a cancellation token, register an action on cancellation.
		if (auto maybeToken = promise.getCancellationToken(); maybeToken)
		{
			executable_ = &promise;
			maybeToken->onTriggered(cancellationObject_);
		}
		return true;
	}

private:
	void onCancel()
	{
		onCancellation_();
		executable_->schedule();
	}

private:
	OnSuspend onSuspend_;
	OnCancellation onCancellation_;
	CancellationCallback cancellationObject_;
	bzd::coroutine::impl::Executable* executable_{nullptr};
};

template <class OnSuspend>
Suspend(OnSuspend&& onSuspend) -> Suspend<OnSuspend, void (*)(void)>;

} // namespace bzd::coroutine::impl
