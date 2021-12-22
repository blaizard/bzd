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

namespace bzd::impl {

template <class T>
using AsyncResultType = typename bzd::typeTraits::RemoveReference<T>::ResultType;

template <class T>
using AsyncOptionalResultType = bzd::Optional<AsyncResultType<T>>;

template <class T>
class Async
{
public: // Traits
	using PromiseType = bzd::coroutine::Promise<T>;
	using ResultType = typename PromiseType::ResultType;
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
	/**
	 * Cancel the current async and nested ones.
	 */
	constexpr void cancel() noexcept
	{
		if (handle_)
		{
			detach();
			handle_.destroy();
			handle_ = nullptr;
		}
	}

	/**
	 * Notifies if the async is completed.
	 */
	[[nodiscard]] constexpr bool isReady() const noexcept { return (handle_) ? handle_.done() : false; }

	/**
	 * Get the current result. If the async is not terminated, an empty value is returned.
	 */
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
			return bzd::move(handle_.promise().result_.valueMutable());
		}
		return nullopt;
	}

	void onTerminate(bzd::FunctionView<void(bzd::Executor::Executable&)> callback) noexcept
	{
		handle_.promise().onTerminateCallback_.emplace(callback);
	}

	constexpr void cancelIfDifferent(const bzd::Executor::Executable& promise) noexcept
	{
		// TODO: fix potential bug if some of the coroutines are executed concurrently,
		// calling cancel on the one running will probably create an UB.
		if (handle_ && static_cast<bzd::Executor::Executable*>(&handle_.promise()) != &promise)
		{
			cancel();
		}
	}

	/**
	 * Detach the current async from its executor (if attached).
	 */
	constexpr void detach() noexcept { bzd::ignore = getExecutable().pop(); }

	/**
	 * Associate an executor to this async.
	 */
	constexpr void setExecutor(bzd::Executor& executor) noexcept { handle_.promise().executor_ = &executor; }

	/**
	 * Associate an executor to this async and push it to the queue.
	 */
	constexpr void enqueue(bzd::Executor& executor) noexcept
	{
		setExecutor(executor);
		// Enqueue the async to the work queue of the executor
		getExecutable().enqueue();
	}

	/**
	 * Run the current async on a given executor.
	 * This call will block until completion of the async.
	 *
	 * \param executor The executor to run on.
	 * \return The result of the async.
	 */
	constexpr ResultType run(bzd::Executor& executor) noexcept
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
		bzd::Executor executor;
		return run(executor);
	}

public: // coroutine specific
	using promise_type = PromiseType;

	constexpr bool await_ready() noexcept { return isReady(); }

	template <class U>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise<U>> caller) noexcept
	{
		auto& promise = handle_.promise();

		// To handle continuation
		promise.caller_ = caller;

		// Push the current handle to the executor.
		promise.enqueue();

		// Returns control to the caller/resumer of the current coroutine,
		// as the current coroutine is already queued for execution.
		return true;
	}

	constexpr ResultType await_resume() noexcept { return bzd::move(moveResultOut().valueMutable()); }

private:
	friend bzd::coroutine::impl::Enqueue;

	constexpr bzd::Executor::Executable& getExecutable() noexcept
	{
		bzd::assert::isTrue(static_cast<bool>(handle_));
		return handle_.promise();
	}

	bzd::coroutine::impl::coroutine_handle<PromiseType> handle_{};
};

} // namespace bzd::impl

namespace bzd {

template <class V, class E = bzd::Error>
class Async : public impl::Async<bzd::Result<V, E>>
{
public:
	using impl::Async<bzd::Result<V, E>>::Async;
};

template <>
class Async<void> : public impl::Async<void>
{
public:
	using impl::Async<void>::Async;
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

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncResultType<Asyncs>...>> all(Asyncs&&... asyncs) noexcept
{
	using ResultType = bzd::Tuple<impl::AsyncResultType<Asyncs>...>;

	// Push all handles to the executor
	(co_await bzd::coroutine::impl::Enqueue{asyncs}, ...);

	// Loop until all asyncs are ready
	while (!(asyncs.isReady() && ...))
	{
		co_await yield();
	}

	// Build the result and return it.
	ResultType result{asyncs.await_resume()...};
	co_return result;
}

template <class... Asyncs>
impl::Async<bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>> any(Asyncs&&... asyncs) noexcept
{
	using ResultType = bzd::Tuple<impl::AsyncOptionalResultType<Asyncs>...>;

	// Install callbacks on terminate.
	// Note: the lifetime of the lambda is longer than the promises.
	auto onTerminateCallback = [&asyncs...](bzd::Executor::Executable& promise) { (asyncs.cancelIfDifferent(promise), ...); };

	// Register on terminate callbacks
	(asyncs.onTerminate(bzd::FunctionView<void(bzd::Executor::Executable&)>{onTerminateCallback}), ...);

	// Push all handles to the executor
	(co_await bzd::coroutine::impl::Enqueue{asyncs}, ...);

	// Loop until one async is ready, the others will be canceled.
	while (!(asyncs.isReady() || ...))
	{
		co_await yield();
	}

	// Build the result and return it.
	ResultType result{asyncs.moveResultOut()...};
	co_return result;
}

} // namespace bzd::async
