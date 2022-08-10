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
			token_ = &maybeToken.valueMutable();
			executable_ = &promise;
			maybeToken->addCallback(cancellationObject_);
		}
		return true;
	}

	constexpr void await_resume() noexcept
	{
		// Remove the callback to ensure the cancellation is not applied to this executable.
		// TODO: Still need to check for race condition here.
		if (token_)
		{
			token_->removeCallback(cancellationObject_);
		}
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
	CancellationToken* token_{nullptr};
	bzd::coroutine::impl::Executable* executable_{nullptr};
};

template <class OnSuspend>
Suspend(OnSuspend&& onSuspend) -> Suspend<OnSuspend, void (*)(void)>;

} // namespace bzd::coroutine::impl
