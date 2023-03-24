#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to detach an async object and optionally execute a callable.
template <Bool forISR, class OnSuspend, class OnCancellation = void (*)(void)>
class Suspend : public bzd::coroutine::impl::suspend_always
{
public: // Constructor.
	constexpr Suspend(OnSuspend&& onSuspend, OnCancellation&& onCancellation) noexcept :
		onSuspend_{bzd::move(onSuspend)}, onCancellation_{bzd::move(onCancellation)}
	{
	}
	constexpr Suspend(OnSuspend&& onSuspend) noexcept : Suspend{bzd::move(onSuspend), []() {}} {}

public: // Coroutine specializations.
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		executable_ = &caller.promise();

		if constexpr (forISR)
		{
			auto callback =
				bzd::FunctionRef<bzd::Optional<Executable&>(void)>::toMember<Suspend, &Suspend::onTerminateCallbackForISR>(*this);
			executable_->thenEnqueueCallback(callback);
		}
		else
		{
			auto callback = bzd::FunctionRef<bzd::Optional<Executable&>(void)>::toMember<Suspend, &Suspend::onTerminateCallback>(*this);
			executable_->thenEnqueueCallback(callback);
		}
		return true;
	}

private:
	bzd::Optional<Executable&> onTerminateCallback() noexcept
	{
		bzd::assert::isTrue(executable_);
		// TO BE FIXED, somehow if a cancellation happens, there is a race.
		// This is maybe because the executable already calls the cancel() callback and
		// schedule the executable in the queue. At the point of calling onSuspend_ for example,
		// the Suspend might alreaddy be destroyed.
		/*if (executable_->isCanceled())
		{
			return *executable_;
		}*/

		auto suspended = executable_->suspend(bzd::FunctionRef<void(void)>{onCancellation_});
		onSuspend_(bzd::move(suspended));
		::std::cout << "SUSPEND!" << ::std::endl;

		// The cancel callback should be set at the end.
		// suspended.addCallback(...);

		// Only here we want the cancellation to be effective, not before.

		// Idea:
		// Make the ExecutableSuspended object a member of this class? and only pass a sort of unique_ptr of it.
		// This should simplify things as there are no need for a complex own(...) function.
		// <- will not work because the object that reschedule need to have a local lifetime detached from this awaitable.

		return bzd::nullopt;
	}

	bzd::Optional<Executable&> onTerminateCallbackForISR() noexcept
	{
		bzd::assert::isTrue(executable_);
		auto suspended = executable_->suspendForISR(bzd::FunctionRef<void(void)>{onCancellation_});
		onSuspend_(bzd::move(suspended));

		return bzd::nullopt;
	}

private:
	OnSuspend onSuspend_;
	OnCancellation onCancellation_;
	bzd::coroutine::impl::Executable* executable_{nullptr};
};

} // namespace bzd::coroutine::impl
