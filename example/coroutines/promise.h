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
};
} // namespace bzd::coroutine::interface

namespace bzd::coroutine::impl {

template <class T>
class Promise : public bzd::coroutine::interface::Promise
{
private:
	struct FinalAwaiter
	{
		constexpr bool await_ready() noexcept { return false; }

		constexpr bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
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
	constexpr Promise() noexcept = default;

	constexpr bzd::coroutine::impl::suspend_always initial_suspend() noexcept { return {}; }

	constexpr FinalAwaiter final_suspend() noexcept
	{
		if (onTerminateCallback_)
		{
			(onTerminateCallback_.value())(*this);
		}
		return {};
	}

	constexpr void unhandled_exception() noexcept { bzd::assert::unreachable(); }

	~Promise() noexcept {}

private:
	template <class U>
	friend class ::bzd::impl::Async;

	bzd::coroutine::impl::coroutine_handle<> caller{nullptr};
	bzd::Optional<bzd::FunctionView<void(interface::Promise&)>> onTerminateCallback_{};
};

} // namespace bzd::coroutine::impl

namespace bzd::coroutine {
template <class T>
class Promise : public impl::Promise<Promise<T>>
{
public:
	using ResultType = T;
	using impl::Promise<Promise<T>>::Promise;

	constexpr auto get_return_object() noexcept { return bzd::coroutine::impl::coroutine_handle<Promise>::from_promise(*this); }

	template <class U>
	constexpr void return_value(U&& result) noexcept
	{
		result_.emplace(bzd::forward<U>(result));
	}

private:
	template <class U>
	friend class ::bzd::impl::Async;

	bzd::Optional<T> result_{};
};

template <>
class Promise<void> : public impl::Promise<Promise<void>>
{
public:
	using ResultType = bool;
	using impl::Promise<Promise<void>>::Promise;

	auto get_return_object() noexcept { return bzd::coroutine::impl::coroutine_handle<Promise>::from_promise(*this); }

	constexpr void return_void() noexcept { result_ = true; }

private:
	template <class U>
	friend class ::bzd::impl::Async;

	bzd::Optional<bool> result_{};
};
} // namespace bzd::coroutine
