#pragma once

#include "example/coroutines/promise.h"
#include "bzd/container/impl/non_owning_list.h"
#include "bzd/utility/singleton.h"
#include "bzd/utility/ignore.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace bzd
{
class Executor : public bzd::Singleton<Executor>
{
public:
	constexpr Executor() = default;

	void push(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise>& handle)
	{
		bzd::ignore = queue_.pushFront(handle.promise());
	}

	bzd::coroutine::impl::coroutine_handle<> pop()
	{
		while (queue_.empty())
		{
			//std::cout << "wait get_next()" << std::endl;
			std::this_thread::sleep_for(100ms);
		}
		auto promise = queue_.back();
		// std::cout << "Executing " << coro.address() << ", count: " << queue.size() << std::endl;
		bzd::ignore = queue_.pop(*promise);

		//std::cout << "next: " << coro.address() << " " << queue.size() << std::endl;

		return bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise>::from_promise(*promise);
	}

private:
	bzd::NonOwningList<bzd::coroutine::Promise> queue_{};
};
}
