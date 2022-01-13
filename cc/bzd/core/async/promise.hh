#pragma once

#include "cc/bzd/container/function_view.hh"
#include "cc/bzd/container/impl/non_owning_list.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/executor.hh"

// Forward declaration
namespace bzd::impl {
template <class T>
class Async;
}

namespace bzd::coroutine::impl {

/**
 * Awaitable to enqueue an async object.
 */
struct Enqueue
{
	template <class T>
	constexpr Enqueue(T& async) noexcept : exectuable_{async.getExecutable()}
	{
	}
	bzd::Executor::Executable& exectuable_;
};

/**
 * Awaitable to yield the current execution.
 */
struct GetExecutor : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
	{
		executor_ = handle.promise().executor_;
		return false;
	}

	constexpr auto await_resume() noexcept { return executor_; }

	bzd::Executor* executor_;
};

/**
 * Awaitable to yield the current execution.
 */
struct Yield : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
	{
		bzd::assert::isTrue(handle.promise().executor_);
		handle.promise().executor_->push(handle.promise());
		return true;
	}
};

template <class T>
class Promise : public bzd::Executor::Executable
{
private:
	using Self = Promise<T>;

private:
	struct FinalAwaiter
	{
		constexpr bool await_ready() noexcept { return false; }

		constexpr bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
		{
			auto continuation = handle.promise().caller_;
			if (continuation)
			{
				return continuation;
			}
			return bzd::coroutine::impl::noop_coroutine();
		}

		constexpr void await_resume() noexcept {}
	};

public:
	constexpr Promise() noexcept :
		bzd::Executor::Executable{bzd::coroutine::impl::coroutine_handle<T>::from_promise(static_cast<T&>(*this))}
	{
	}

	constexpr Promise(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Promise(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;
	~Promise() noexcept = default;

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

	constexpr auto await_transform(bzd::coroutine::impl::Enqueue&& object) noexcept
	{
		auto& exectuable = object.exectuable_;
		exectuable.executor_ = executor_;
		exectuable.enqueue();
		return bzd::coroutine::impl::suspend_never{};
	}

	constexpr auto&& await_transform(bzd::coroutine::impl::Yield&& awaitable) noexcept { return bzd::move(awaitable); }
	constexpr auto&& await_transform(bzd::coroutine::impl::GetExecutor&& awaitable) noexcept { return bzd::move(awaitable); }

	template <class Async>
	constexpr auto&& await_transform(Async&& async) noexcept
	{
		// NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
		async.setExecutor(*executor_);
		return bzd::move(async);
	}

	/*
		void* operator new(std::size_t size)
		{
			void* ptr = ::malloc(size);
			::std::cout << "Allocating " << size << " " << ptr << ::std::endl;
			if (!ptr) throw std::bad_alloc{};
			return ptr;
		}

		void operator delete(void* ptr, std::size_t size)
		{
			::std::cout << "Deallocating " << size << " " << ptr << ::std::endl;
			::free(ptr);
		}
	*/

private:
	template <class U>
	friend class ::bzd::impl::Async;
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
