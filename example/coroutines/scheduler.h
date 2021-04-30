#pragma once

#include "example/coroutines/promise.h"
#include "bzd/container/impl/non_owning_list.h"
#include "bzd/utility/singleton.h"
#include "bzd/utility/ignore.h"

namespace bzd
{
class Scheduler : public bzd::Singleton<Scheduler>
{
public:
	constexpr Scheduler() = default;

	void push(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise>& handle)
	{
		bzd::ignore = queue_.pushFront(handle.promise());
	}

	bzd::coroutine::impl::coroutine_handle<> pop()
	{
		while (queue_.empty())
		{
			// Call wait callback
		}
		auto promise = queue_.back();
		bzd::ignore = queue_.pop(*promise);

		return bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise>::from_promise(*promise);
	}

private:
	bzd::NonOwningList<bzd::coroutine::Promise> queue_{};
};
}
