#pragma once

#include "example/coroutines/coroutine.h"
#include "cc/bzd/container/impl/non_owning_list.h"

#include <functional>

namespace bzd::coroutine
{

class Promise : public bzd::NonOwningListElement<true>
{
public:	
	constexpr Promise() = default;

	auto get_return_object() { return bzd::coroutine::impl::coroutine_handle<Promise>::from_promise(*this); }

	bzd::coroutine::impl::suspend_always initial_suspend() { return {}; }

	struct FinalAwaiter
	{
		bool await_ready() noexcept { return false; }

		bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<Promise> handle) noexcept
		{
			auto& continuation = handle.promise().caller;
			if (continuation)
			{
				return continuation;
			}
			return bzd::coroutine::impl::noop_coroutine();
		}

		void await_resume() noexcept {}
	};

	FinalAwaiter final_suspend()
	{
		callback_();
		return {};
	}

	void return_value(int)
	{
		//  std::cout << "got " << 23 << "\n";
	}

	void unhandled_exception() {}

	~Promise() = default;

	bzd::coroutine::impl::coroutine_handle<> caller{nullptr};
	std::function<void(void)> callback_ = []() {};
};

}
