#pragma once

#include "example/coroutines/coroutine.h"
#include "example/coroutines/promise.h"
#include "example/coroutines/executor.h"

#include "bzd/utility/ignore.h"

namespace bzd
{
struct Async
{
	using promise_type = bzd::coroutine::Promise;
	Async(bzd::coroutine::impl::coroutine_handle<promise_type> h) : handle(h)
	{
		//std::cout << "Async " << handle.address() << std::endl;
	}

	~Async()
	{
		//std::cout << "~Async" << handle.address() << std::endl;
		if (handle)
		{
			bzd::ignore = handle.promise().pop();
			handle.destroy();
		}
	}

	int sync()
	{
		// std::cout << "We got asked for the return value..." << std::endl;
		while (!handle.done())
		{
			//std::cout << "sync" << std::endl;
			handle.resume();
		}
		return 42; //bzd::move(handle.promise().value);
	}

	// Cancel the current coroutine and nested ones
	void cancel()
	{
		if (handle)
		{
			handle.destroy();
			handle = nullptr;
		}
	}

	bool isReady() const noexcept
	{
		return handle.done();
	}

	void set_callback(std::function<void(void)> callback)
	{
		handle.promise().callback_ = callback;
	}

	bool await_ready()
	{
		return false; //handle.done();
	}

	auto await_suspend(bzd::coroutine::impl::coroutine_handle<> caller)
	{
		//std::cout << "await_suspend caller " << caller.address() << ", handle " << handle.address() << std::endl;
		handle.promise().caller = caller;

		// Must be valid as there is at least a single entry
		// The coroutine should be added to the list when it resolves
		//std::cout << "await_suspend" << std::endl;
		Executor::getInstance().push(handle);

		//std::cout << "get_next() " << handle.address() << std::endl;
		return Executor::getInstance().pop();
	}

	auto await_resume()
	{
		// std::cout << "Await value is returned: " << 23 << std::endl;
		return 23;
	}

private:
	static auto always_await()
	{
		struct Awaitable : public bzd::coroutine::impl::suspend_always
		{
			auto await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise> b)
			{
				bzd::Executor::getInstance().push(b);
				return bzd::Executor::getInstance().pop();
			}
		};

		return Awaitable{};
	}

public:
	static Async promiseAnd(Async& a, Async& b)
	{
		bzd::Executor::getInstance().push(a.handle);
		bzd::Executor::getInstance().push(b.handle);
		while (!a.isReady() || !b.isReady())
		{
			//std::cout << "promiseAnd" << std::endl;
			//auto coro = get_next();
			//coro.resume();
			co_await always_await();
			//co_await suspend_always();
			std::this_thread::sleep_for(10ms);
			//std::cout << a.done() << "  " << b.done() << std::endl;
		}

		co_return 42;
	}

	static Async promiseOr(Async& a, Async& b)
	{
		a.set_callback([&b]() {
			std::cout << "terminated A" << std::endl;
			b.cancel();
		});

		b.set_callback([&a]() {
			std::cout << "terminated B" << std::endl;
			a.cancel();
		});

		//std::cout << "promiseOr START" << std::endl;
		bzd::Executor::getInstance().push(a.handle);
		bzd::Executor::getInstance().push(b.handle);
		while (!a.isReady() && !b.isReady())
		{
			//std::cout << "promiseOr" << std::endl;
			//auto coro = get_next();
			//coro.resume();
			co_await always_await();
			//co_await suspend_always();
			std::this_thread::sleep_for(10ms);
		}

		// register_delay_coro for example.
		std::cout << "end promiseOr" << std::endl;

		co_return 42;
	}

	bzd::coroutine::impl::coroutine_handle<promise_type> handle;
};
}
