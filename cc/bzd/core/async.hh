#pragma once

#include "cc/bzd/container/function_view.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

#include <iostream>

namespace bzd::impl {

template <class T>
using AsyncResultType = typename bzd::typeTraits::RemoveReference<T>::ResultType;

template <class T>
using AsyncOptionalResultType = bzd::Optional<AsyncResultType<T>>;

using AsyncExecutor = bzd::coroutine::impl::Executor;
using AsyncExecutable = bzd::coroutine::impl::Executable;

template <class T>
class Async
{
public: // Traits
	using PromiseType = bzd::coroutine::Promise<T>;
	using ResultType = typename PromiseType::ResultType;
	using Executable = bzd::coroutine::impl::Executable;
	using Executor = AsyncExecutor;
	using Self = Async<T>;

public: // constructor/destructor/assignments
	constexpr Async(bzd::coroutine::impl::coroutine_handle<PromiseType> h) noexcept : handle_(h) {}

	constexpr Async(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;

	constexpr Async(Self&& async) noexcept : handle_{bzd::move(async.handle_)} { async.handle_ = nullptr; }
	constexpr Self& operator=(Self&& async) noexcept
	{
		handle_ = bzd::move(async.handle_);
		async.handle_ = nullptr;
	}

	~Async() noexcept
	{
		// Detach it from where it is and destroy the handle.
		if (handle_)
		{
			cancel();
		}
	}

public:
	/// Cancel the current async and nested ones.
	///
	/// This function is not thread safe and must be called on an async object non active.
	constexpr void cancel() noexcept
	{
		if (handle_)
		{
			detach();
			handle_.destroy();
			handle_ = nullptr;
		}
	}

	/// Notifies if the async is completed.
	[[nodiscard]] constexpr bool isReady() const noexcept { return (handle_) ? handle_.done() : false; }

	[[nodiscard]] constexpr bool isCanceled() const noexcept { return (handle_) ? handle_.promise().isCanceled() : false; }

	[[nodiscard]] constexpr bool isReadyOrCanceled() const noexcept { return isReady() || isCanceled(); }

	/// Get the current result. If the async is not terminated, an empty value is returned.
	[[nodiscard]] constexpr bzd::Optional<ResultType> getResult() noexcept
	{
		if (handle_)
		{
			return handle_.promise().result_;
		}
		return nullopt;
	}

	[[nodiscard]] constexpr bzd::Optional<ResultType> moveResultOut() noexcept
	{
		if (handle_)
		{
			return bzd::move(handle_.promise().result_);
		}
		return nullopt;
	}

	/// Detach the current async from its executor (if attached).
	constexpr void detach() noexcept { bzd::ignore = getExecutable().pop(); }

	/// Associate an executor to this async.
	constexpr void setExecutor(Executor& executor) noexcept { handle_.promise().setExecutor(executor); }

	/// Associate an executor to this async and push it to the queue.
	constexpr void enqueue(Executor& executor) noexcept
	{
		setExecutor(executor);
		// Enqueue the async to the work queue of the executor
		getExecutable().enqueue();
	}

	/// Run the current async on a given executor.
	/// This call will block until completion of the async.
	///
	/// \param executor The executor to run on.
	/// \return The result of the async.
	constexpr ResultType run(Executor& executor) noexcept
	{
		// Associate the executor with this async and enqueue it.
		enqueue(executor);

		// Run the executor
		executor.run();

		// Return the result.
		return await_resume();
	}

	constexpr ResultType sync() noexcept
	{
		Executor executor;
		return run(executor);
	}

	constexpr void setCancellationToken(interface::CancellationToken& token) noexcept { handle_.promise().setCancellationToken(token); }

public: // coroutine specific
	using promise_type = PromiseType;

	constexpr bool await_ready() noexcept { return isReadyOrCanceled(); }

	template <class U>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise<U>> caller) noexcept
	{
		auto& promise = handle_.promise();

		// To handle continuation
		promise.caller_ = &caller.promise();

		// Push the current handle to the executor.
		promise.enqueue();

		// Returns control to the caller/resumer of the current coroutine,
		// as the current coroutine is already queued for execution.
		return true;
	}

	constexpr ResultType await_resume() noexcept { return bzd::move(moveResultOut().valueMutable()); }

	// private:
	friend bzd::coroutine::impl::Enqueue;

	constexpr Executable& getExecutable() noexcept
	{
		bzd::assert::isTrue(static_cast<bool>(handle_));
		return handle_.promise();
	}

	bzd::coroutine::impl::coroutine_handle<PromiseType> handle_{};
};

} // namespace bzd::impl

namespace bzd {

template <class V = void, class E = bzd::Error>
class Async : public impl::Async<bzd::Result<V, E>>
{
public:
	using impl::Async<bzd::Result<V, E>>::Async;
};

} // namespace bzd

namespace bzd::async {

constexpr auto yield() noexcept
{
	return bzd::coroutine::impl::Yield{};
}

constexpr auto getExecutor() noexcept
{
	return bzd::coroutine::impl::GetExecutor{};
}

constexpr auto getExecutable() noexcept
{
	return bzd::coroutine::impl::GetExecutable{};
}

/// Wait for the next scheduler iteration for all running schedulers.
inline Async<> yieldAll() noexcept
{
	auto& executor = *(co_await bzd::async::getExecutor());
	const auto currentTick{executor.getNextTick()};
	auto running = executor.getRangeRunning();
	for (const auto& element : running)
	{
		// Keeping track of the previous tick is needed here to handle cases where the counter wraps.
		auto tick = element.getTick();
		auto previousTick = tick;
		while (tick < currentTick && previousTick != tick && !element.isCompleted())
		{
			co_await bzd::async::yield();
			previousTick = tick;
			tick = element.getTick();
		}
	}

	co_return {};
}

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncResultType<Asyncs>...>> all(Asyncs&&... asyncs) noexcept
{
	using ResultType = bzd::Tuple<impl::AsyncResultType<Asyncs>...>;

	// Push all handles to the executor
	(co_await bzd::coroutine::impl::Enqueue{asyncs}, ...);

	// Loop until all asyncs are ready
	while (!(asyncs.isReady() && ...))
	{
		// Suspend this coroutine now, it will re-enter once one of the pending task
		// is completed.
		co_await bzd::coroutine::impl::Suspend();
	}

	// Build the result and return it.
	ResultType result{asyncs.await_resume()...};
	co_return result;
}

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>> any(Asyncs&&... asyncs) noexcept
{
	using ResultType = bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>;

	bzd::Atomic<UInt8Type> flag{0};
	bzd::interface::CancellationToken token{flag};
	(asyncs.setCancellationToken(token), ...);

	// Register on terminate callbacks.
	bzd::Atomic<BoolType> enqueueCaller{true};
	auto onTerminateCallback = [&](impl::AsyncExecutable* caller) -> impl::AsyncExecutable* {
		// Ensure that only one instance return to the caller.
		// This prevents concurrent thread to enque the caller, which coudl lead to a situation
		// where the caller is executed (not in the queue) and being enqued at the same time,
		// so will be executed twice and potentially in parallel.
		if (enqueueCaller.exchange(false))
		{
			// No need to clear other async callers and callbacks as if which the scope,
			// they will return a null continuation and this function cannot be called out of scope
			// because we wait for all executors to yeild once before returning.
			token.trigger();
			return caller;
		}
		return nullptr;
	};
	(co_await bzd::coroutine::impl::Enqueue{asyncs, onTerminateCallback}, ...);

	// The first coroutine that complete will re-enter here.
	co_await bzd::coroutine::impl::Suspend();

	// Wait until no executors are processing cancelled coroutines.
	// by waiting for at least one iteration from all executors.
	co_await bzd::async::yieldAll();

	// By now all asyncs must have been either ready or canceled.
	// Build the result and return it.
	ResultType result{asyncs.moveResultOut()...};
	co_return result;
}

} // namespace bzd::async
