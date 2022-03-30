#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/core/async/awaitables/enqueue.hh"
#include "cc/bzd/core/async/awaitables/get_executable.hh"
#include "cc/bzd/core/async/awaitables/get_executor.hh"
#include "cc/bzd/core/async/awaitables/yield.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

#include <iostream>

namespace bzd::impl {

template <class T>
class Async
{
public: // Traits
	using PromiseType = bzd::coroutine::Promise<T>;
	using promise_type = PromiseType; // Needed for the corountine compiler hooks.
	using ResultType = typename PromiseType::ResultType;
	using Executable = bzd::coroutine::impl::Executable;
	using Executor = bzd::coroutine::impl::Executor;
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
			destroy();
		}
	}

public:
	/// Notifies if the async is completed.
	[[nodiscard]] constexpr bool isReady() const noexcept { return (handle_) ? handle_.done() : false; }

	[[nodiscard]] constexpr bool isCanceled() const noexcept { return (handle_) ? handle_.promise().isCanceled() : false; }

	[[nodiscard]] constexpr bool isCompleted() const noexcept { return isReady() || isCanceled(); }

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

	constexpr auto assert()
	{
		class AsyncPropagate : public impl::Async<T>
		{
		public:
			constexpr AsyncPropagate(impl::Async<T>&& async) noexcept : impl::Async<T>{bzd::move(async)}
			{
				this->handle_.promise().setPropagate();
			}

			constexpr typename ResultType::Value await_resume() noexcept
			{
				auto result{bzd::move(impl::Async<T>::await_resume())};
				bzd::assert::isTrue(result.hasValue());
				return bzd::move(result.valueMutable());
			}
		};
		return AsyncPropagate{bzd::move(*this)};
	}

	/// Detach the current async from its executor (if attached).
	constexpr void detach() noexcept { bzd::ignore = getExecutable().popToDiscard(); }

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
		auto result = run(executor);
		destroy();
		return bzd::move(result);
	}

	constexpr void setCancellationToken(CancellationToken& token) noexcept { handle_.promise().setCancellationToken(token); }

public: // coroutine specific
	constexpr bool await_ready() noexcept { return isReady(); }

	template <class U>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<bzd::coroutine::Promise<U>> caller) noexcept
	{
		// caller ()
		// {
		//    co_await this ()
		/// }
		auto& promise = handle_.promise();
		auto& promiseCaller = caller.promise();

		// Propagate the executor and cancellation if any
		promiseCaller.propagateContextTo(promise);

		// To handle continuation
		promise.caller_ = &promiseCaller;

		// Push the current handle to the executor.
		promise.enqueue();

		// Returns control to the caller/resumer of the current coroutine,
		// as the current coroutine is already queued for execution.
		return true;
	}

	constexpr ResultType await_resume() noexcept
	{
		bzd::assert::isTrue(isReady());
		return bzd::move(moveResultOut().valueMutable());
	}

private:
	template <class... Args>
	friend class bzd::coroutine::impl::Enqueue;

	template <class U>
	friend class bzd::coroutine::impl::Promise;

	/// Destroy the current async and nested ones.
	///
	/// This function is not thread safe and must be called on an async object non active.
	constexpr void destroy() noexcept
	{
		if (handle_)
		{
			detach();
			handle_.destroy();
			handle_ = nullptr;
		}
	}

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

namespace bzd::concepts {
template <class T>
concept async = sameTemplate<T, bzd::Async> || sameTemplate<T, bzd::impl::Async>;
}

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

template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...) impl::Async<typename bzd::coroutine::impl::EnqueueAll<Asyncs...>::ResultType> all(Asyncs&&... asyncs)
noexcept
{
	co_return(co_await bzd::coroutine::impl::EnqueueAll<Asyncs...>{bzd::forward<Asyncs>(asyncs)...});
}

template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...) impl::Async<typename bzd::coroutine::impl::EnqueueAny<Asyncs...>::ResultType> any(Asyncs&&... asyncs)
noexcept
{
	co_return(co_await bzd::coroutine::impl::EnqueueAny<Asyncs...>{bzd::forward<Asyncs>(asyncs)...});
}

} // namespace bzd::async
