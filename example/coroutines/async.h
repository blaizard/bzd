#pragma once

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
} // namespace bzd::impl

namespace bzd {
class Async
{
public:
	using ResultType = bzd::Result<int, int>;
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

	int sync()
	{
		while (!isReady())
		{
			// std::cout << "sync" << std::endl;
			handle_.resume();
		}
		return 42; // bzd::move(handle.promise().value);
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

	void set_callback(std::function<void(void)> callback) { handle_.promise().callback_ = callback; }

	bool await_ready() { return isReady(); }

	auto await_suspend(bzd::coroutine::impl::coroutine_handle<> caller)
	{
		// To handle continuation
		handle_.promise().caller = caller;

		// Push the current handle to the scheduler and pop the next one.
		Scheduler::getInstance().push(handle_);
		return Scheduler::getInstance().pop();
	}

	auto await_resume()
	{
		return handle_.promise().result_;
	}

public:
	bzd::coroutine::impl::coroutine_handle<promise_type> handle_;
};
template <class T>
using Type = bzd::Optional<typename bzd::typeTraits::RemoveReference<T>::ResultType>;

template <class... Asyncs>
static Async waitAll(Asyncs&&... asyncs)
{
	using ResultType = bzd::Tuple<Type<Asyncs>...>;

	ResultType ret{};
	(void)ret;

	// Push all handles to the scheduler
	(bzd::Scheduler::getInstance().push(asyncs.handle_), ...);

	// Loop until all asyncs are ready
	while (!(asyncs.isReady() && ...))
	{
		co_await bzd::impl::SuspendAlways<Async::ResultType>{};
	}

	co_return 42;
}

static Async waitAny(Async& a, Async& b)
{
	a.set_callback([&b]() { b.cancel(); });

	b.set_callback([&a]() { a.cancel(); });

	bzd::Scheduler::getInstance().push(a.handle_);
	bzd::Scheduler::getInstance().push(b.handle_);
	while (!a.isReady() && !b.isReady())
	{
		co_await bzd::impl::SuspendAlways<Async::ResultType>{};
	}

	co_return 42;
}
} // namespace bzd
