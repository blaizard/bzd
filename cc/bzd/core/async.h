#pragma once

#include "cc/bzd/container/function_view.h"
#include "cc/bzd/container/optional.h"
#include "cc/bzd/container/result.h"
#include "cc/bzd/container/tuple.h"
#include "cc/bzd/core/coroutine.h"
#include "cc/bzd/core/promise.h"
#include "cc/bzd/core/scheduler.h"
#include "cc/bzd/type_traits/remove_reference.h"
#include "cc/bzd/utility/ignore.h"

namespace bzd::impl {
struct SuspendAlways : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise<T>> handle) noexcept
	{
		bzd::Scheduler::getInstance().push(handle);
		return true;
	}
};

template <class T>
using AsyncResultType = typename bzd::typeTraits::RemoveReference<T>::ResultType;

template <class T>
using AsyncOptionalResultType = bzd::Optional<AsyncResultType<T>>;

template <class T>
class Async
{
public:
	using PromiseType = bzd::coroutine::Promise<T>;
	using ResultType = typename PromiseType::ResultType;

public: // constructor/destructor
	constexpr Async(bzd::coroutine::impl::coroutine_handle<PromiseType> h) noexcept : handle_(h) {}

	~Async() noexcept
	{
		// Detach it from where it is and destroy the handle.
		if (handle_)
		{
			cancel();
		}
	}

public:
	// Cancel the current coroutine and nested ones
	constexpr void cancel() noexcept
	{
		if (handle_)
		{
			detach();
			handle_.destroy();
			handle_ = nullptr;
		}
	}

	constexpr bool isReady() const noexcept { return (handle_) ? handle_.done() : false; }

	constexpr bzd::Optional<ResultType> getResult() noexcept
	{
		if (handle_)
		{
			return handle_.promise().result_;
		}
		return nullopt;
	}

	void onTerminate(bzd::FunctionView<void(bzd::coroutine::interface::Promise&)> callback) noexcept
	{
		handle_.promise().onTerminateCallback_.emplace(callback);
	}

	constexpr void cancelIfDifferent(const bzd::coroutine::interface::Promise& promise) noexcept
	{
		if (handle_ && static_cast<bzd::coroutine::interface::Promise*>(&handle_.promise()) != &promise)
		{
			cancel();
		}
	}

	/**
	 * Detach the current async from its scheduler (if attached).
	 */
	constexpr void detach() noexcept
	{
		bzd::assert::isTrue(static_cast<bool>(handle_));
		bzd::ignore = handle_.promise().pop();
	}

	/**
	 * Attach the current async to the scheduler.
	 */
	constexpr void attach() noexcept
	{
		bzd::assert::isTrue(static_cast<bool>(handle_));
		bzd::Scheduler::getInstance().push(handle_);
	}

public: // coroutine specific
	using promise_type = PromiseType;

	constexpr bool await_ready() noexcept { return isReady(); }

	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<> caller) noexcept
	{
		// To handle continuation
		handle_.promise().caller = caller;

		// Push the current handle to the scheduler.
		attach();

		// Returns control to the caller/resumer of the current coroutine,
		// as the current coroutine is already queued for execution.
		return true;
	}

	constexpr ResultType await_resume() noexcept { return getResult().value(); }

private:
	bzd::coroutine::impl::coroutine_handle<PromiseType> handle_;
};

} // namespace bzd::impl

namespace bzd {

template <class V, class E = bzd::BoolType>
class Async : public impl::Async<bzd::Result<V, E>>
{
public:
	using impl::Async<bzd::Result<V, E>>::Async;
};

template <>
class Async<void> : public impl::Async<void>
{
public:
	using impl::Async<void>::Async;
};

} // namespace bzd

namespace bzd::async {

constexpr auto yield() noexcept
{
	return bzd::impl::SuspendAlways{};
}

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncResultType<Asyncs>...>> all(Asyncs&&... asyncs) noexcept
{
	using ResultType = bzd::Tuple<impl::AsyncResultType<Asyncs>...>;

	// Push all handles to the scheduler
	(asyncs.attach(), ...);

	// Loop until all asyncs are ready
	while (!(asyncs.isReady() && ...))
	{
		co_await yield();
	}

	// Build the result and return it.
	ResultType result{asyncs.getResult().value()...};
	co_return result;
}

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>> any(Asyncs&&... asyncs) noexcept
{
	using ResultType = bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>;

	// Install callbacks on terminate.
	// Note: the lifetime of the lambda is longer than the promises.
	auto onTerminateCallback = [&asyncs...](bzd::coroutine::interface::Promise& promise) { (asyncs.cancelIfDifferent(promise), ...); };

	// Register on terminate callbacks
	(asyncs.onTerminate(bzd::FunctionView<void(bzd::coroutine::interface::Promise&)>{onTerminateCallback}), ...);
	// Push all handles to the scheduler
	(asyncs.attach(), ...);

	// Loop until one async is ready
	while (!(asyncs.isReady() || ...))
	{
		co_await yield();
	}

	// Build the result and return it.
	ResultType result{asyncs.getResult()...};
	co_return result;
}

} // namespace bzd::async
