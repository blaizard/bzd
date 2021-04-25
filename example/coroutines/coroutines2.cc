#include "example/coroutines/coroutine.h"

#include <atomic>
#include <chrono>
#include <deque>
#include <iostream>
#include <thread>
#include <functional>

using namespace std::chrono_literals;

static std::deque<bzd::coroutine::impl::coroutine_handle<>> queue;
static std::atomic<bool> isEnd{false};

void register_coro(bzd::coroutine::impl::coroutine_handle<>& handle)
{
	//std::cout << "Registering " << handle.address() << std::endl;
	queue.push_back(handle);
}

class RAIIThread
{
public:
	template <class T>
	RAIIThread(T&& fct) : t_{fct}
	{
	}

	~RAIIThread() { t_.join(); }

private:
	std::thread t_;
};

void register_delay_coro(bzd::coroutine::impl::coroutine_handle<>& handle)
{
	static std::deque<bzd::coroutine::impl::coroutine_handle<>> tempQueue;
	static RAIIThread thr{[]() {
		while (!isEnd)
		{
			std::this_thread::sleep_for(20ms);
			if (!tempQueue.empty())
			{
				auto coro = tempQueue.front();
				tempQueue.pop_front();
				register_coro(coro);
			}
		}
	}};

	tempQueue.push_back(handle);
}

bzd::coroutine::impl::coroutine_handle<> get_next()
{
	// Wait until there is something in the queue
	while (queue.empty())
	{
		//std::cout << "wait get_next()" << std::endl;
		std::this_thread::sleep_for(100ms);
	}
	auto coro = queue.front();
	// std::cout << "Executing " << coro.address() << ", count: " << queue.size() << std::endl;
	queue.pop_front();

	//std::cout << "next: " << coro.address() << " " << queue.size() << std::endl;

	return coro;
}

struct Async
{
	struct Promise
	{
		constexpr Promise() = default;

		Async get_return_object() { return bzd::coroutine::impl::coroutine_handle<Promise>::from_promise(*this); }
		bzd::coroutine::impl::suspend_always initial_suspend() { return {}; }

		struct final_awaiter
		{
			bool await_ready() noexcept { return false; }
			bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<Promise> h) noexcept
			{
				auto& continuation = h.promise().caller;
				if (continuation)
				{
					//std::cout << "continuation: " << continuation.address() << std::endl;
					return continuation;
				}
				return bzd::coroutine::impl::noop_coroutine();
			}

			void await_resume() noexcept {}
		};

		final_awaiter final_suspend()
		{
			callback_();
			return {};
		}
		void return_value(int)
		{
			//  std::cout << "got " << 23 << "\n";
		}

		void unhandled_exception() {}

		~Promise()
		{
			//std::cout << "~Promise" << std::endl;
		}

		bzd::coroutine::impl::coroutine_handle<> caller{nullptr};
		std::function<void(void)> callback_ = []() {};
	};

	using promise_type = Promise;
	Async(bzd::coroutine::impl::coroutine_handle<Promise> h) : handle(h)
	{
		//std::cout << "Async " << handle.address() << std::endl;
	}

	~Async()
	{
		//std::cout << "~Async" << handle.address() << std::endl;
		if (handle)
		{
			handle.destroy();
		}

		// Remove from queue
		for (auto it = queue.begin(); it != queue.end(); )
		{
			if (it->address() == handle.address())
			{
				//std::cout << "remove " << handle.address() << std::endl;
				it = queue.erase(it);
			}
			else
			{
				++it;
			}
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
		register_coro(handle);

		//std::cout << "get_next() " << handle.address() << std::endl;
		return get_next();
	}

	auto await_resume()
	{
		// std::cout << "Await value is returned: " << 23 << std::endl;
		return 23;
	}

	bzd::coroutine::impl::coroutine_handle<Promise> handle;
};

auto always_await()
{
	struct Awaitable : bzd::coroutine::impl::suspend_always
	{
		auto await_suspend(bzd::coroutine::impl::coroutine_handle<> b)
		{
			register_coro(b);
			return get_next();
		}
	};

	return Awaitable{};
}

Async promiseTest(bzd::coroutine::impl::coroutine_handle<> a)
{
	register_coro(a);
	if (!a.done())
	{
		//std::cout << "promiseTest" << std::endl;
		//auto coro = get_next();
		//coro.resume();
		co_await always_await();
		std::cout << a.done() << std::endl;
	}
	//std::cout << "end promiseTest" << std::endl;

	co_return 42;
}

Async promiseAnd(bzd::coroutine::impl::coroutine_handle<> a, bzd::coroutine::impl::coroutine_handle<> b)
{
	register_coro(a);
	register_coro(b);
	while (!a.done() || !b.done())
	{
		//std::cout << "promiseAnd" << std::endl;
		//auto coro = get_next();
		//coro.resume();
		co_await always_await();
		//co_await suspend_always();
		std::this_thread::sleep_for(10ms);
		//std::cout << a.done() << "  " << b.done() << std::endl;
	}
	std::cout << "end promiseAnd" << std::endl;

	co_return 42;
}

Async promiseOr(bzd::coroutine::impl::coroutine_handle<> a, bzd::coroutine::impl::coroutine_handle<> b)
{
	//std::cout << "promiseOr START" << std::endl;
	register_coro(a);
	register_coro(b);
	while (!a.done() && !b.done())
	{
		//std::cout << "promiseOr" << std::endl;
		//auto coro = get_next();
		//coro.resume();
		co_await always_await();
		//co_await suspend_always();
		std::this_thread::sleep_for(10ms);
	}

	//if (!a.done())
	//	a.destroy();
	//if (!b.done())
	//	b.destroy();
	// Remove a and b from list
	// note, sub coroutines also needs to be deleted, like the ones in
	// register_delay_coro for example.
	std::cout << "end promiseOr" << std::endl;

	co_return 42;
}

Async nop()
{
	co_return 42;
}

Async count(int id, int n)
{
	int sum = 0;
	// std::cout << id << ": count(" << n << ")" << std::endl;
	for (int i = 0; i < n; ++i)
	{
		std::cout << id << ": " << i << std::endl;

		auto coro_nop = nop();

		if (n-i > 5)
		{
			auto coro_count1 = count(id * 10 + 1, n - 3);
			auto coro_count2 = count(id * 10, n - 1);
			co_await promiseAnd(coro_count1.handle, coro_count2.handle);
		}
		else
		{
			sum += co_await coro_nop;
		}
	}

	std::cout << id << " end: " << sum << std::endl;
	co_return sum;
}

// https://lewissbaker.github.io/2020/05/11/understanding_symmetric_transfer
// Another https://stackoverflow.com/questions/41413489/c1z-coroutine-threading-context-and-coroutine-scheduling
// Paper: https://luncliff.github.io/coroutine/ppt/[Eng]ExploringTheCppCoroutine.pdf
int main()
{
	{
		auto mycoro1 = count(1, 0);

		auto promise = promiseTest(mycoro1.handle);
		promise.sync();
	}

	std::cout << "-----------------" << std::endl;

	{
		auto mycoro1 = count(1, 3);
		auto mycoro2 = count(2, 6);

		auto promise = promiseAnd(mycoro1.handle, mycoro2.handle);
		promise.sync();
	}

	std::cout << "-----------------" << std::endl;

	{
		auto mycoro1 = count(1, 3);
		auto mycoro2 = count(2, 6);

		mycoro1.set_callback([&mycoro2]() {
			std::cout << "terminated A" << std::endl;
			mycoro2.cancel();
		});

		mycoro2.set_callback([&mycoro1]() {
			std::cout << "terminated B" << std::endl;
			mycoro1.cancel();
		});

		auto promise = promiseOr(mycoro1.handle, mycoro2.handle);
		promise.sync();
	}

	std::cout << "-----------------" << std::endl;

	isEnd.store(true);
	return 0;
}
