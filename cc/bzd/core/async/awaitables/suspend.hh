#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::async::awaitable {

/// Creates an ExecutableSuspended object for sharing.
///
/// This object activates the ExecutableSuspended on destruction.
template <class T>
class ExecutableSuspendedFactory : public bzd::async::impl::ExecutableSuspended<T>
{
public:
	constexpr ExecutableSuspendedFactory(T& executable, const bzd::FunctionRef<void(void)> onCancel) noexcept :
		bzd::async::impl::ExecutableSuspended<T>{}, executableRaw_{executable}, ownerStorage_{this}
	{
		this->executable_.store(&executable);
		this->onUserCancel_ = onCancel;
		this->owner_ = &ownerStorage_;
	}
	ExecutableSuspendedFactory(const ExecutableSuspendedFactory&) = delete;
	ExecutableSuspendedFactory& operator=(const ExecutableSuspendedFactory&) = delete;
	ExecutableSuspendedFactory(ExecutableSuspendedFactory&&) = delete;
	ExecutableSuspendedFactory& operator=(ExecutableSuspendedFactory&&) = delete;
	constexpr ~ExecutableSuspendedFactory() noexcept { this->activate(executableRaw_); }

private:
	T& executableRaw_;
	bzd::async::impl::ExecutableSuspended<bzd::async::impl::PromiseBase>* ownerStorage_;
}; // namespace bzd::coroutine::impl

/// Awaitable to detach an async object and optionally execute a callable.
template <class OnSuspend, class OnCancellation = void (*)(void)>
class Suspend : public bzd::async::impl::suspend_always
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
	constexpr bool await_suspend(bzd::async::impl::coroutine_handle<T> caller) noexcept
	{
		promise_ = &caller.promise();

		auto callback =
			bzd::FunctionRef<bzd::Optional<bzd::async::impl::PromiseBase&>(void)>::toMember<Suspend, &Suspend::onTerminateCallback>(*this);
		promise_->thenEnqueueCallback(callback);

		return true;
	}

private:
	bzd::Optional<bzd::async::impl::PromiseBase&> onTerminateCallback() noexcept
	{
		bzd::assert::isTrue(promise_);

		promise_->skip();
		auto suspended =
			ExecutableSuspendedFactory<bzd::async::impl::PromiseBase>{*promise_, bzd::FunctionRef<void(void)>{onCancellation_}};
		onSuspend_(static_cast<bzd::async::impl::ExecutableSuspended<bzd::async::impl::PromiseBase>&&>(suspended));

		return bzd::nullopt;
	}

private:
	OnSuspend onSuspend_;
	OnCancellation onCancellation_;
	bzd::async::impl::PromiseBase* promise_{nullptr};
};

} // namespace bzd::async::awaitable
