#pragma once

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
			bzd::ignore = handle_.promise().pop();
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
			handle_.destroy();
			handle_ = nullptr;
		}
	}

	bool isReady() const noexcept { return handle_.done(); }

	ResultType& getResult() noexcept { return handle_.promise().result_.valueMutable(); }

	void set_callback(std::function<void(void)> callback) { handle_.promise().callback_ = callback;}

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
	//using ResultType = bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>;

/*	(asyncs.set_callback([&](void* addr){
		for (auto& async : {asyncs...}) {
			if (&async != addr) {
				//const_cast<Async&>(async).cancel();
			}
		} 
	}), ...);*/

	Async* as[]{&asyncs...};
	as[0]->set_callback([&as]() { as[1]->cancel(); });
	as[1]->set_callback([&as]() { as[0]->cancel(); });

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
