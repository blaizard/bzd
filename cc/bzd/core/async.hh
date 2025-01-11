#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/core/async/awaitables/default.hh"
#include "cc/bzd/core/async/awaitables/enqueue.hh"
#include "cc/bzd/core/async/awaitables/get_executable.hh"
#include "cc/bzd/core/async/awaitables/get_executor.hh"
#include "cc/bzd/core/async/awaitables/no_return.hh"
#include "cc/bzd/core/async/awaitables/propagate.hh"
#include "cc/bzd/core/async/awaitables/suspend.hh"
#include "cc/bzd/core/async/awaitables/yield.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/forward.hh"
#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd::async {
using Executable = bzd::async::impl::PromiseBase;
using ExecutableSuspended = bzd::async::impl::ExecutableSuspended<Executable>;
using Executor = bzd::async::impl::Executor<Executable>;
using Type = bzd::async::impl::ExecutableMetadata::Type;
} // namespace bzd::async

namespace bzd::impl {

template <class T>
struct AsyncTaskTraits
{
	static constexpr typeTraits::AsyncType type{typeTraits::AsyncType::task};
	static constexpr Bool isReentrant = false;
	using PromiseType = bzd::async::PromiseTask<T>;
};

template <class T>
struct AsyncGeneratorTraits
{
	static constexpr typeTraits::AsyncType type{typeTraits::AsyncType::generator};
	static constexpr Bool isReentrant = true;
	using PromiseType = bzd::async::PromiseGenerator<T>;
};

template <class T, template <class> class Traits>
class [[nodiscard]] Async
{
public: // Traits
	using PromiseType = typename Traits<T>::PromiseType;
	using promise_type = PromiseType; // Needed for the corountine compiler hooks.
	using ResultType = typename PromiseType::ResultType;
	using Self = Async;
	static constexpr typeTraits::AsyncType type{Traits<T>::type};

public: // constructor/destructor/assignments
	constexpr Async(bzd::async::impl::coroutine_handle<PromiseType> h) noexcept : handle_(h) {}

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
	[[nodiscard]] constexpr bool hasResult() const noexcept { return (handle_) ? handle_.promise().hasResult() : false; }

	[[nodiscard]] constexpr bool isCanceled() const noexcept { return (handle_) ? handle_.promise().isCanceled() : false; }

	[[nodiscard]] constexpr bool isCompleted() const noexcept
	{
		return (handle_) ? (handle_.promise().isCompleted() || handle_.promise().isCanceled()) : false;
	}

	[[nodiscard]] constexpr ResultType moveResultOut() noexcept
	{
		bzd::assert::isTrue(hasResult());
		return bzd::move(moveOptionalResultOut().valueMutable());
	}

	[[nodiscard]] constexpr ResultType& result() noexcept
	{
		bzd::assert::isTrue(hasResult());
		return handle_.promise().result().valueMutable();
	}

	template <bzd::Size index = 0>
	constexpr auto assertHasValue() noexcept
	{
		// Not compatible with re-entrant function because this moves the asynchronous into a wrapper,
		// therefore the original async looses the ownership.
		static_assert(!Traits<T>::isReentrant, "Error propagation is not compatible with re-entrant asyncs.");
		this->handle_.promise().setPropagate(
			PromiseType::PropagateErrorCallback::template toMember<Self, &Self::hasErrorToPropagate<index>>(*this));

		struct AsyncPropagate
		{
		public:
			constexpr explicit AsyncPropagate(Self& async) noexcept : async_{async} {}

			constexpr auto operator co_await() noexcept { return bzd::async::awaitable::Propagate<Async, index>(async_); }

		private:
			Self& async_;
		};

		return AsyncPropagate{*this};
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
			executor.run(/*coreUId*/ 0u);
			// Here should run the idle task.
		} while (!hasResult());

		// Return the result.
		return moveResultOut();
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
	constexpr auto operator co_await() noexcept { return bzd::async::awaitable::Awaiter<Async>(*this); }

private:
	template <class... Args>
	friend class bzd::async::awaitable::Enqueue;
	template <class... Args>
	friend class bzd::async::awaitable::EnqueueAny;
	template <class U>
	friend class bzd::async::awaitable::Awaiter;

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

	template <bzd::Size index>
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

	constexpr async::Executable& getExecutable() noexcept
	{
		bzd::assert::isTrue(static_cast<bool>(handle_));
		return handle_.promise();
	}

	[[nodiscard]] constexpr bzd::Optional<ResultType> moveOptionalResultOut() noexcept
	{
		if (handle_)
		{
			return handle_.promise().moveOptionalResultOut();
		}
		return nullopt;
	}

private:
	bzd::async::impl::coroutine_handle<PromiseType> handle_{};
};

} // namespace bzd::impl

namespace bzd {

template <class V, class E>
class Async : public impl::Async<bzd::Result<V, E>, impl::AsyncTaskTraits>
{
public:
	using impl::Async<bzd::Result<V, E>, impl::AsyncTaskTraits>::Async;
	using Value = V;
	using Error = E;
};

} // namespace bzd

namespace bzd::iterator {

template <class T>
class Generator : public typeTraits::IteratorBase
{
public: // Traits.
	using ValueType = typename T::Value;
	using DifferenceType = bzd::Int32;
	static constexpr auto category = typeTraits::IteratorCategory::input;
	struct Sentinel
	{
		[[nodiscard]] constexpr Bool operator==(const Generator other) const noexcept { return other == *this; }
		[[nodiscard]] constexpr Bool operator!=(const Generator other) const noexcept { return !(other == *this); }
	};

public: // Constructors
	constexpr explicit Generator(T& generator) noexcept : generator_{generator} {}

public: // API
	bzd::Async<> operator++() noexcept
	{
		co_await generator_;
		if (generator_.hasResult())
		{
			if (generator_.result().hasError())
			{
				co_return bzd::move(generator_.moveResultOut()).propagate();
			}
		}
		co_return {};
	}

public: // Comparators.
	[[nodiscard]] constexpr Bool operator==(const Sentinel) const noexcept { return generator_.isCompleted(); }
	[[nodiscard]] constexpr Bool operator!=(const Sentinel sentinel) const noexcept { return !(*this == sentinel); }

public: // Accessors.
	[[nodiscard]] constexpr auto& operator*() const { return generator_.result().valueMutable(); }
	[[nodiscard]] constexpr auto* operator->() const { return &(generator_.result().valueMutable()); }

private:
	T& generator_;
};

} // namespace bzd::iterator

namespace bzd {

template <class V, class E>
class Generator : public impl::Async<bzd::Result<V, E>, impl::AsyncGeneratorTraits>
{
public:
	using impl::Async<bzd::Result<V, E>, impl::AsyncGeneratorTraits>::Async;
	using Value = V;
	using Error = E;
	using Iterator = bzd::iterator::Generator<Generator<V, E>>;

public: // coroutine specific
	constexpr auto operator co_await() noexcept { return bzd::async::awaitable::AwaiterNoReturn<Generator>(*this); }

public:
	bzd::Async<Iterator> begin() noexcept
	{
		Iterator it{*this};
		// Only the first time, process the coroutine to reach the first suspension point (either a co_yield or a co_return)
		if (first_)
		{
			co_await !++it;
			first_ = false;
		}
		co_return it;
	}

	constexpr typename Iterator::Sentinel end() const noexcept { return {}; }

private:
	bool first_{true};
};

} // namespace bzd

namespace bzd::async {

template <concepts::asyncGenerator T, class Callable>
bzd::Async<> forEach(T&& generator, Callable&& callable) noexcept
{
	auto it = co_await !generator.begin();
	while (it != generator.end())
	{
		using CallableReturnType = typeTraits::InvokeResult<Callable, decltype(*it)>;
		if constexpr (concepts::asyncCallable<Callable, decltype(*it)>)
		{
			if constexpr (concepts::sameClassAs<CallableReturnType, bzd::Async<>>)
			{
				co_await !callable(*it);
			}
			else if constexpr (concepts::sameClassAs<CallableReturnType, bzd::Async<bool>>)
			{
				if (!(co_await !callable(*it)))
				{
					break;
				}
			}
			else
			{
				static_assert(bzd::meta::alwaysFalse<T>,
							  "Callback return type not supported, only bzd::Async<> or bzd::Async<bool> is supported.");
			}
		}
		else
		{
			if constexpr (concepts::sameClassAs<CallableReturnType, void>)
			{
				callable(*it);
			}
			else if constexpr (concepts::sameClassAs<CallableReturnType, bool>)
			{
				if (!callable(*it))
				{
					break;
				}
			}
			else
			{
				static_assert(bzd::meta::alwaysFalse<T>, "Callback return type not supported, only void or bool is supported.");
			}
		}
		co_await !++it;
	}

	co_return {};
}

constexpr auto yield() noexcept { return bzd::async::awaitable::Yield{}; }

constexpr auto getExecutor() noexcept { return bzd::async::awaitable::GetExecutor{}; }

constexpr auto getExecutable() noexcept { return bzd::async::awaitable::GetExecutable{}; }

/// Create a suspension point. Mark the current async as skipped, it will still reside
/// in the executor workload queue but will not be processed.
///
/// This is suitable for ISR.
template <class... Args>
constexpr auto suspend(Args&&... args) noexcept
{
	return bzd::async::awaitable::Suspend{bzd::forward<Args>(args)...};
}

/// Executes multiple asynchronous function according to the executor policy and return once all are completed.
template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...)::bzd::impl::Async<typename bzd::async::awaitable::EnqueueAll<Asyncs...>::ResultType, ::bzd::impl::AsyncTaskTraits>
all(Asyncs&&... asyncs) noexcept
{
	co_return (co_await bzd::async::awaitable::EnqueueAll<Asyncs...>{/*parallel*/ false, bzd::forward<Asyncs>(asyncs)...});
}

/// Executes multiple asynchronous function according to the executor policy and return once all are completed.
template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...)::bzd::impl::Async<typename bzd::async::awaitable::EnqueueAll<Asyncs...>::ResultType, ::bzd::impl::AsyncTaskTraits>
allParallel(Asyncs&&... asyncs) noexcept
{
	co_return (co_await bzd::async::awaitable::EnqueueAll<Asyncs...>{/*parallel*/ true, bzd::forward<Asyncs>(asyncs)...});
}

/// Executes multiple asynchronous function according to the executor policy and return once at least one of them is completed.
template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...)::bzd::impl::Async<typename bzd::async::awaitable::EnqueueAny<Asyncs...>::ResultType, ::bzd::impl::AsyncTaskTraits>
any(Asyncs&&... asyncs) noexcept
{
	co_return (co_await bzd::async::awaitable::EnqueueAny<Asyncs...>{/*parallel*/ false, bzd::forward<Asyncs>(asyncs)...});
}

/// Executes multiple asynchronous function according to the executor policy and return once at least one of them is completed.
template <concepts::async... Asyncs>
requires(!concepts::lValueReference<Asyncs> &&
		 ...)::bzd::impl::Async<typename bzd::async::awaitable::EnqueueAny<Asyncs...>::ResultType, ::bzd::impl::AsyncTaskTraits>
anyParallel(Asyncs&&... asyncs) noexcept
{
	co_return (co_await bzd::async::awaitable::EnqueueAny<Asyncs...>{/*parallel*/ true, bzd::forward<Asyncs>(asyncs)...});
}

} // namespace bzd::async
