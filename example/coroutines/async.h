#pragma once

#include "bzd/container/array.h"
#include "bzd/container/function_view.h"
#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/type_traits/remove_reference.h"
#include "bzd/utility/ignore.h"
#include "example/coroutines/coroutine.h"
#include "example/coroutines/promise.h"
#include "example/coroutines/scheduler.h"

namespace bzd::impl {
template <class T>
struct SuspendAlways : public bzd::coroutine::impl::suspend_always
{
	auto await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise<T>> handle)
	{
		bzd::Scheduler::getInstance().push(handle);
		return bzd::Scheduler::getInstance().pop();
	}
};

template <class T = bzd::Result<int, int>>
class Async
{
public:
	using ResultType = T;
	using promise_type = bzd::coroutine::Promise<ResultType>;

public:
	constexpr Async(bzd::coroutine::impl::coroutine_handle<promise_type> h) : handle_(h) {}

	~Async()
	{
		// Detach it from where it is and destroy the handle.
		if (handle_)
		{
			cancel();
		}
	}

	ResultType sync()
	{
		while (!isReady())
		{
			handle_.resume();
		}
		return getResult();
	}

	// Cancel the current coroutine and nested ones
	void cancel()
	{
		if (handle_)
		{
			bzd::ignore = handle_.promise().pop();
			handle_.destroy();
			handle_ = nullptr;
		}
	}

	bool isReady() const noexcept { return (handle_) ? handle_.done() : false; }

	ResultType& getResult() noexcept { return handle_.promise().result_.valueMutable(); }

	bool await_ready() { return isReady(); }

	auto await_suspend(bzd::coroutine::impl::coroutine_handle<> caller)
	{
		// To handle continuation
		handle_.promise().caller = caller;

		// Push the current handle to the scheduler and pop the next one.
		Scheduler::getInstance().push(handle_);
		return Scheduler::getInstance().pop();
	}

	ResultType await_resume() { return getResult(); }

	void onTerminate(bzd::FunctionView<void(bzd::coroutine::interface::Promise&)> callback)
	{
		handle_.promise().onTerminateCallback_ = callback;
	}

	bool isSame(bzd::coroutine::interface::Promise& promise) const noexcept
	{
		return (handle_ && static_cast<bzd::coroutine::interface::Promise*>(&handle_.promise()) == &promise);
	}

public:
	bzd::coroutine::impl::coroutine_handle<promise_type> handle_;
};

template <class T>
using AsyncResultType = typename bzd::typeTraits::RemoveReference<T>::ResultType;

template <class T>
using AsyncOptionalResultType = bzd::Optional<AsyncResultType<T>>;

} // namespace bzd::impl

namespace bzd {

class Async : public impl::Async<>
{
public:
	using impl::Async<>::Async;
};

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncResultType<Asyncs>...>> waitAll(Asyncs&&... asyncs)
{
	using ResultType = bzd::Tuple<impl::AsyncResultType<Asyncs>...>;

	// Push all handles to the scheduler
	(bzd::Scheduler::getInstance().push(asyncs.handle_), ...);

	// Loop until all asyncs are ready
	while (!(asyncs.isReady() && ...))
	{
		co_await bzd::impl::SuspendAlways<ResultType>{};
	}

	// Build the result and return it.
	ResultType result{asyncs.getResult()...};
	co_return result;
}

template <class... Asyncs>
static Async waitAny(Asyncs&&... asyncs)
{
	// using ResultType = bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>;

	// Install callbacks on terminate, note the lifetime of the array and the callback
	// is longer than the promises.
	const bzd::Array<Async*, sizeof...(asyncs)> async_list{&asyncs...};
	auto onTerminateCallback = [&async_list](bzd::coroutine::interface::Promise& promise) {
		for (auto async : async_list)
		{
			if (!async->isSame(promise))
			{
				async->cancel();
			}
		}
	};
	const bzd::FunctionView<void(bzd::coroutine::interface::Promise&)> temp{onTerminateCallback};
	for (auto async : async_list)
	{
		async->onTerminate(temp);
	}

	// Push all handles to the scheduler
	(bzd::Scheduler::getInstance().push(asyncs.handle_), ...);

	// Loop until one async is ready
	while (!(asyncs.isReady() || ...))
	{
		co_await bzd::impl::SuspendAlways<Async::ResultType>{};
	}

	co_return 42;
}

} // namespace bzd
