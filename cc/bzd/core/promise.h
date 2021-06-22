#pragma once

#include "cc/bzd/container/function_view.h"
#include "cc/bzd/container/impl/non_owning_list.h"
#include "cc/bzd/container/optional.h"
#include "cc/bzd/core/coroutine.h"

#include <iostream>

// Forward declaration
namespace bzd::impl {
template <class T>
class Async;
}
namespace bzd {
	class Scheduler;
}

namespace bzd::coroutine::interface {
class Promise : public bzd::NonOwningListElement<true>
{
};
} // namespace bzd::coroutine::interface

namespace bzd::coroutine::impl {

struct SuspendAlways : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
	{
		bzd::assert::isTrue(handle.promise().executor_);
		handle.promise().executor_->push(handle);
		return true;
	}
};

template <class T>
struct Attach
{
	Attach(T& async) : async_{async} {}
	T& async_;
};

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

/*
	template <class Awaitable>
	auto&& await_transform(Awaitable&& awaitable)
	{
		return bzd::move(awaitable);
	}
*/
	template <class U>
	auto&& await_transform(bzd::impl::Async<U>&& awaitable)
	{
		// Associate the async with the same executor as the current prommise.
		awaitable.setExecutor(*executor_);

		return bzd::move(awaitable);
	}

	auto&& await_transform(bzd::coroutine::impl::SuspendAlways&& awaitable)
	{
		return bzd::move(awaitable);
	}

	template <class U>
	bzd::coroutine::impl::suspend_never await_transform(bzd::coroutine::impl::Attach<U>&& attach)
	{
		// Associate the async with the same executor as the current prommise.
		attach.async_.setExecutor(*executor_);
		attach.async_.queue();
		return {};
	}

	~Promise() noexcept {}

private:
	template <class U>
	friend class ::bzd::impl::Async;

	friend struct ::bzd::coroutine::impl::SuspendAlways;

	bzd::coroutine::impl::coroutine_handle<> caller{nullptr};
	bzd::Scheduler* executor_{nullptr};
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
