#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Creates an ExecutableSuspended object for sharing.
///
/// This object activates the ExecutableSuspended on destruction.
template <class T>
class ExecutableSuspendedFactory : public bzd::interface::ExecutableSuspended<T>
{
public:
	constexpr ExecutableSuspendedFactory(T& executable, const bzd::FunctionRef<void(void)> onCancel) noexcept :
		bzd::interface::ExecutableSuspended<T>{}, executableRaw_{executable}
	{
		this->executable_.store(&executable);
		this->onUserCancel_ = onCancel;
	}
	ExecutableSuspendedFactory(const ExecutableSuspendedFactory&) = delete;
	ExecutableSuspendedFactory& operator=(const ExecutableSuspendedFactory&) = delete;
	ExecutableSuspendedFactory(ExecutableSuspendedFactory&&) = delete;
	ExecutableSuspendedFactory& operator=(ExecutableSuspendedFactory&&) = delete;
	constexpr ~ExecutableSuspendedFactory() noexcept
	{
		this->activate(executableRaw_);
	}

private:
	T& executableRaw_;
};

/// Awaitable to detach an async object and optionally execute a callable.
template <class OnSuspend, class OnCancellation = void (*)(void)>
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

		auto callback =
			bzd::FunctionRef<bzd::Optional<Executable&>(void)>::toMember<Suspend, &Suspend::onTerminateCallback>(*this);
		executable_->thenEnqueueCallback(callback);

		return true;
	}

private:
	bzd::Optional<Executable&> onTerminateCallback() noexcept
	{
		bzd::assert::isTrue(executable_);

		executable_->skip();
		auto suspended = impl::ExecutableSuspendedFactory<Executable>{
			*executable_, bzd::FunctionRef<void(void)>{onCancellation_}
		};
		onSuspend_(bzd::move(suspended));

		return bzd::nullopt;
	}

private:
	OnSuspend onSuspend_;
	OnCancellation onCancellation_;
	bzd::coroutine::impl::Executable* executable_{nullptr};
};

} // namespace bzd::coroutine::impl
