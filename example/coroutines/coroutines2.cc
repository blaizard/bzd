#include "cc/bzd/utility/ignore.h"
#include "example/coroutines/async.h"

#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

static std::atomic<bool> isEnd{false};

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
/*
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
*/

bzd::Async<int, int> nop()
{
	co_return 42;
}

bzd::Async<int, int> count(int id, int n)
{
	int sum = 0;
	// std::cout << id << ": count(" << n << ")" << std::endl;
	for (int i = 0; i < n; ++i)
	{
		std::cout << id << ": " << i << std::endl;

		auto coro_nop = nop();

		if (n - i > 5)
		{
			auto coro_count1 = count(id * 10 + 1, n - 3);
			auto coro_count2 = count(id * 10, n - 1);
			co_await bzd::async::all(coro_count1, coro_count2);
		}
		else
		{
			sum += (co_await coro_nop).value();
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
	std::cout << "-----------------" << std::endl;

	{
		auto mycoro1 = count(1, 3);
		auto mycoro2 = count(2, 6);

		auto promise = bzd::async::all(mycoro1, mycoro2);
		promise.sync();
	}

	std::cout << "-----------------" << std::endl;

	{
		auto mycoro1 = count(1, 3);
		auto mycoro2 = count(2, 6);

		auto promise = bzd::async::any(mycoro1, mycoro2);
		promise.sync();
	}

	std::cout << "-----------------" << std::endl;

	isEnd.store(true);
	return 0;
}
