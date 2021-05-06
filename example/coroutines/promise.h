#pragma once

#include "bzd/container/impl/non_owning_list.h"
#include "bzd/container/optional.h"
#include "bzd/container/function_view.h"
#include "example/coroutines/coroutine.h"

namespace bzd::coroutine::interface {
class Promise : public bzd::NonOwningListElement<true>
{
public:
	constexpr Promise() = default;
};
} // namespace bzd::coroutine::interface

namespace bzd::coroutine {

template <class T>
class Promise : public bzd::coroutine::interface::Promise
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
		if (onTerminateCallback_)
		{
			(onTerminateCallback_.value())(*this);
		}
		return {};
	}

	template <class U>
	void return_value(U&& result) noexcept
	{
		result_.emplace(bzd::forward<U>(result));
	}

	void unhandled_exception() {}

	~Promise() = default;

	bzd::coroutine::impl::coroutine_handle<> caller{nullptr};
	bzd::Optional<bzd::FunctionView<void(interface::Promise&)>> onTerminateCallback_{};

	bzd::Optional<T> result_{};
};

} // namespace bzd::coroutine
