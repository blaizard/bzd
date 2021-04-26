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

}
