#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/core/async/awaitables/enqueue.hh"
#include "cc/bzd/core/async/awaitables/get_executable.hh"
#include "cc/bzd/core/async/awaitables/get_executor.hh"
#include "cc/bzd/core/async/awaitables/suspend.hh"
#include "cc/bzd/core/async/awaitables/yield.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd::async {
using Executable = bzd::coroutine::impl::Executable;
using ExecutableRef = bzd::interface::ExecutableRef<bzd::interface::Executable<bzd::coroutine::impl::Executable>>;
using Executor = bzd::coroutine::impl::Executor;
using Type = bzd::ExecutableMetadata::Type;
} // namespace bzd::async

namespace bzd::impl {

struct AsyncTag
{
};

template <class T>
class [[nodiscard]] Async
{
public: // Traits
	using PromiseType = bzd::coroutine::Promise<T>;
	using promise_type = PromiseType; // Needed for the corountine compiler hooks.
	using ResultType = typename PromiseType::ResultType;
	using Self = Async<T>;
	using Tag = bzd::impl::AsyncTag;

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
	[[nodiscard]] constexpr bool isReady() const noexcept { return (handle_) ? handle_.promise().isReady() : false; }

	[[nodiscard]] constexpr bool isCanceled() const noexcept { return (handle_) ? handle_.promise().isCanceled() : false; }

	[[nodiscard]] constexpr bool isCompleted() const noexcept { return isReady() || isCanceled(); }

	[[nodiscard]] constexpr bzd::Optional<ResultType> moveResultOut() noexcept
	{
		if (handle_)
		{
			return bzd::move(handle_.promise()).moveResultOut();
		}
		return nullopt;
	}

	template <bzd::Size index = 0>
	constexpr auto assertHasValue() noexcept
	{
		class AsyncPropagate : public impl::Async<T>
		{
		public:
			using Self = AsyncPropagate;
			using Parent = impl::Async<T>;

		public:
			constexpr AsyncPropagate(impl::Async<T>&& async) noexcept : Parent{bzd::move(async)}
			{
				this->handle_.promise().setPropagate(
					PromiseType::PropagateErrorCallback::template toMember<Self, &Self::hasErrorToPropagate>(*this));
			}

			bzd::Optional<bzd::Error> hasErrorToPropagate() noexcept
			{
				// It must have a value, get a reference to it.
				auto& result{this->handle_.promise().result().valueMutable()};
				if constexpr (PromiseType::resultTypeIsResult)
				{
					static_assert(index == 0, "The index used with assert must be 0 for non-tuple-like results.");

					if (result.hasError())
					{
						return bzd::move(result.errorMutable());
					}
				}
				else if constexpr (PromiseType::resultTypeIsTupleOfOptionalResultsWithError)
				{
					static_assert(index < PromiseType::ResultType::size(), "The index used with assert is out of bound.");

					// Return if one of the result is an error
					for (auto& variant : result)
					{
						auto maybeError = variant.match([](auto& value) -> bzd::Error* {
							if (value.hasValue() && value.value().hasError())
							{
								return &(value.valueMutable().errorMutable());
							}
							return nullptr;
						});
						if (maybeError)
						{
							return bzd::move(*maybeError);
						}
					}
					// If not check that there is a value at the expected index.
					if (!result.template get<index>().hasValue())
					{
						return bzd::Error{bzd::SourceLocation::current(),
										  bzd::ErrorType::failure,
										  "Got a value but not for the expected async #{}."_csv,
										  index};
					}
				}
				else
				{
					static_assert(bzd::meta::alwaysFalse<T>, "This type of Async cannot be used with assert(...).");
				}

				return bzd::nullopt;
			}

			constexpr auto await_resume() noexcept
			{
				if constexpr (PromiseType::resultTypeIsResult)
				{
					auto result{bzd::move(Parent::await_resume())};
					bzd::assert::isTrue(result.hasValue());
					return bzd::move(result.valueMutable());
				}
				else if constexpr (PromiseType::resultTypeIsTupleOfOptionalResultsWithError)
				{
					auto tuple{bzd::move(Parent::await_resume())};
					auto& maybeResult{tuple.template get<index>()};
					bzd::assert::isTrue(maybeResult.hasValue());
					bzd::assert::isTrue(maybeResult.value().hasValue());
					return bzd::move(maybeResult.valueMutable().valueMutable());
				}
				else
				{
					static_assert(bzd::meta::alwaysFalse<T>, "This type of Async cannot be used with assert(...).");
				}
			}
		};
		return AsyncPropagate{bzd::move(*this)};
	}

	constexpr auto operator!() noexcept { return assertHasValue<0>(); }

	/// Associate an executor to this async and push it to the queue.
	constexpr void enqueue(async::Executor& executor,
						   const bzd::async::Type type = bzd::async::Type::workload,
						   const bzd::Optional<async::Executable::OnTerminateCallback> continuation = bzd::nullopt) noexcept
	{
		auto& executable{getExecutable()};
		if (continuation)
		{
			executable.setConditionalContinuation(continuation.value());
		}
		executor.schedule(executable, type);
	}

	/// Run the current async on a given executor.
	/// This call will block until completion of the async.
	///
	/// \param executor The executor to run on.
	/// \return The result of the async.
	constexpr ResultType run(async::Executor& executor) noexcept
	{
		// Associate the executor with this async and enqueue it.
		enqueue(executor);

		do
		{
			// Run the executor.
			executor.run();
			// Here should run the idle task.
		} while (!isReady());

		// Return the result.
		return await_resume();
	}

	/// Execute this coroutine on a free-standing executor.
	constexpr ResultType sync() noexcept
	{
		async::Executor executor;
		auto result = run(executor);
		// Needed as the result is expected to get recovered only after the coroutine is destroyed.
		destroy();
		return bzd::move(result);
	}

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

		// Propagate the context of the caller to this async.
		promiseCaller.propagate(promise);

		// Handle the continuation.
		promise.setContinuation(promiseCaller);

		// Enqueue the current handle to the executor.
		promiseCaller.thenEnqueueExecutable(promise);

		// Returns control to the caller of the current coroutine,
		// as the current coroutine is already queued for execution.
		return true;
	}

	[[nodiscard]] constexpr ResultType await_resume() noexcept
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
			getExecutable().destroy();
			handle_.destroy();
			handle_ = nullptr;
		}
	}

	constexpr async::Executable& getExecutable() noexcept
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
concept async = sameAs<typename T::Tag, bzd::impl::AsyncTag>;
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

template <class... Args>
constexpr auto suspend(Args&&... args) noexcept
{
	return bzd::coroutine::impl::Suspend{bzd::forward<Args>(args)...};
}

/// Executes multiple asynchronous function according to the executor policy and return once all are completed.
template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...) impl::Async<typename bzd::coroutine::impl::EnqueueAll<Asyncs...>::ResultType> all(Asyncs&&... asyncs)
noexcept
{
	co_return (co_await bzd::coroutine::impl::EnqueueAll<Asyncs...>{bzd::forward<Asyncs>(asyncs)...});
}

/// Executes multiple asynchronous function according to the executor policy and return once at least one of them is completed.
template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...) impl::Async<typename bzd::coroutine::impl::EnqueueAny<Asyncs...>::ResultType> any(Asyncs&&... asyncs)
noexcept
{
	co_return (co_await bzd::coroutine::impl::EnqueueAny<Asyncs...>{bzd::forward<Asyncs>(asyncs)...});
}

} // namespace bzd::async
