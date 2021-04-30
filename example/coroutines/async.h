#pragma once

#include "example/coroutines/coroutine.h"
#include "example/coroutines/promise.h"
#include "example/coroutines/scheduler.h"

#include "bzd/utility/ignore.h"

namespace bzd::impl
{
	template <class V, class E>
	struct SuspendAlways : public bzd::coroutine::impl::suspend_always
	{
		auto await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise<V, E>> handle)
		{
			bzd::Scheduler::getInstance().push(handle);
			return bzd::Scheduler::getInstance().pop();
		}
	};
}

namespace bzd
{
struct Async
{
public:
	using promise_type = bzd::coroutine::Promise<int, int>;
	using ValueType = int;

public:
	constexpr Async(bzd::coroutine::impl::coroutine_handle<promise_type> h) : handle_(h)
	{
	}

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
			//std::cout << "sync" << std::endl;
			handle_.resume();
		}
		return 42; //bzd::move(handle.promise().value);
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

	bool isReady() const noexcept
	{
		return handle_.done();
	}

	void set_callback(std::function<void(void)> callback)
	{
		handle_.promise().callback_ = callback;
	}

	bool await_ready()
	{
		return isReady();
	}

	auto await_suspend(bzd::coroutine::impl::coroutine_handle<> caller)
	{
		//std::cout << "await_suspend caller " << caller.address() << ", handle " << handle.address() << std::endl;
		handle_.promise().caller = caller;

		// Must be valid as there is at least a single entry
		// The coroutine should be added to the list when it resolves
		//std::cout << "await_suspend" << std::endl;
		Scheduler::getInstance().push(handle_);

		//std::cout << "get_next() " << handle.address() << std::endl;
		return Scheduler::getInstance().pop();
	}

	auto await_resume()
	{
		// std::cout << "Await value is returned: " << 23 << std::endl;
		return 23;
	}

public:
	bzd::coroutine::impl::coroutine_handle<promise_type> handle_;
};

template <class... Asyncs>
static Async waitAll(Asyncs&... asyncs)
{
	//using ValueType = bzd::Tuple<Asyncs::ValueType>;

	// Push all handles to the scheduler
	(bzd::Scheduler::getInstance().push(asyncs.handle_), ...);

	// Loop until all asyncs are ready
	while (!(asyncs.isReady() && ...))
	{
		co_await bzd::impl::SuspendAlways<int, int>{};
	}

	co_return 42;
}

static Async waitAny(Async& a, Async& b)
{
	a.set_callback([&b]() {
		b.cancel();
	});

	b.set_callback([&a]() {
		a.cancel();
	});

	bzd::Scheduler::getInstance().push(a.handle_);
	bzd::Scheduler::getInstance().push(b.handle_);
	while (!a.isReady() && !b.isReady())
	{
		co_await bzd::impl::SuspendAlways<int, int>{};
	}

	co_return 42;
}
}
