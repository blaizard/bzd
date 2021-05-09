#pragma once

#include "bzd/container/function_view.h"
#include "bzd/container/impl/non_owning_list.h"
#include "bzd/container/optional.h"
#include "example/coroutines/coroutine.h"

// Forward declaration
namespace bzd::impl {
template <class T>
class Async;
}

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
private:
	struct FinalAwaiter
	{
		constexpr bool await_ready() noexcept { return false; }

		constexpr bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<Promise> handle) noexcept
		{
			auto& continuation = handle.promise().caller;
			if (continuation)
			{
				return continuation;
			}
			return bzd::coroutine::impl::noop_coroutine();
		}

		constexpr void await_resume() noexcept {}
	};

public:
	constexpr Promise() = default;

	constexpr auto get_return_object() { return bzd::coroutine::impl::coroutine_handle<Promise>::from_promise(*this); }

	constexpr bzd::coroutine::impl::suspend_always initial_suspend() { return {}; }

	constexpr FinalAwaiter final_suspend()
	{
		if (onTerminateCallback_)
		{
			(onTerminateCallback_.value())(*this);
		}
		return {};
	}

	template <class U>
	constexpr void return_value(U&& result) noexcept
	{
		result_.emplace(bzd::forward<U>(result));
	}

	constexpr void unhandled_exception() noexcept {}

	~Promise() = default;

private:
	template <class U>
	friend class ::bzd::impl::Async;

	bzd::coroutine::impl::coroutine_handle<> caller{nullptr};
	bzd::Optional<bzd::FunctionView<void(interface::Promise&)>> onTerminateCallback_{};
	bzd::Optional<T> result_{};
};

} // namespace bzd::coroutine
