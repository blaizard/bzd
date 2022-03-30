#pragma once

#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/utility/apply.hh"

namespace bzd::impl {
template <class T>
class Async;
}

namespace bzd::coroutine::impl {

/// Awaitable to enqueue an async object.
template <class... Asyncs>
class Enqueue : public bzd::coroutine::impl::suspend_always
{
public:
	using Self = Enqueue<Asyncs...>;
	template <class T>
	using ResultTypeHelper = typename bzd::typeTraits::RemoveReference<T>::ResultType;

protected:
	template <class... Ts>
	constexpr Enqueue(bzd::FunctionRef<bool(void)> callback, Ts&&... asyncs) noexcept :
		callback_{callback}, asyncs_{inPlace, bzd::forward<Ts>(asyncs)...}
	{
	}

	constexpr Enqueue(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Enqueue(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

	constexpr void enqueue(Executable& caller, bzd::Optional<CancellationToken&> maybeToken) noexcept
	{
		constexprForContainerInc(asyncs_, [&](auto& async) {
			auto& executable = async.getExecutable();
			if (maybeToken.hasValue())
			{
				executable.setCancellationToken(maybeToken.valueMutable());
			}
			executable.onTerminateCallback_.emplace(callback_);
			executable.caller_ = &caller;
			executable.setExecutor(caller.getExecutor());
			executable.enqueue();
		});
	}

protected:
	bzd::FunctionRef<bool(void)> callback_;
	bzd::Tuple<Asyncs...> asyncs_;
};

template <class... Asyncs>
class EnqueueAll : public Enqueue<Asyncs...>
{
public: // Traits.
	using Self = EnqueueAll<Asyncs...>;
	using ResultType = bzd::Tuple<typename Enqueue<Asyncs...>::template ResultTypeHelper<Asyncs>...>;

public: // Constructor.
	template <class... Ts>
	constexpr EnqueueAll(Ts&&... asyncs) noexcept :
		Enqueue<Asyncs...>{bzd::FunctionRef<bool(void)>::toMember<Self, &Self::onTerminateCallback>(*this), bzd::forward<Ts>(asyncs)...}
	{
	}

public: // Coroutine specializations.
	template <class T>
	constexpr void await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		auto& promise = caller.promise();
		this->enqueue(promise, promise.getCancellationToken());
	}

	constexpr ResultType await_resume() noexcept
	{
		return bzd::apply(
			[](auto&... asyncs) -> ResultType {
				// Wait for all async to be ready, this is needed in case the terminate callback
				// is called concurrenlty and the caller is enqueued before all asyncs are completed.
				while (((!asyncs.isReady()) || ...))
					;
				// Copy the result of each async into the result type and return it.
				return ResultType{bzd::inPlace, asyncs.await_resume()...};
			},
			this->asyncs_);
	}

private:
	constexpr bool onTerminateCallback() noexcept
	{
		// Atomically count the number of async completed, and only for the last one,
		// push the caller back into the scheduling queue.
		// This makes this design thread safe.
		return (++counter_ == sizeof...(Asyncs));
	}

private:
	bzd::Atomic<SizeType> counter_{0};
};

template <class... Asyncs>
class EnqueueAny : public Enqueue<Asyncs...>
{
public: // Traits.
	using Self = EnqueueAny<Asyncs...>;
	using ResultType = bzd::Tuple<bzd::Optional<typename Enqueue<Asyncs...>::template ResultTypeHelper<Asyncs>>...>;

public: // Constructor.
	template <class... Ts>
	constexpr EnqueueAny(Ts&&... asyncs) noexcept :
		Enqueue<Asyncs...>{bzd::FunctionRef<bool(void)>::toMember<Self, &Self::onTerminateCallback>(*this), bzd::forward<Ts>(asyncs)...}
	{
	}

public: // Coroutine specializations.
	template <class T>
	constexpr void await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		auto& promise = caller.promise();

		// This defines the cancellation to pass to the executables.
		auto maybeToken = promise.getCancellationToken();
		// Attach the token to the parent if both exists.
		if (maybeToken.hasValue())
		{
			token_.attachTo(maybeToken.valueMutable());
		}

		this->enqueue(promise, token_);
	}

	constexpr ResultType await_resume() noexcept
	{
		return bzd::apply(
			[](auto&... asyncs) -> ResultType {
				// Wait for all async to be completed, this is needed in case the terminate callback
				// is called concurrenlty and the caller is enqueued before all asyncs are completed.
				while (((!asyncs.isCompleted()) || ...))
					;
				// By now all asyncs must have been either ready or canceled.
				// Build the result and return it.
				return ResultType{bzd::inPlace, asyncs.moveResultOut()...};
			},
			this->asyncs_);
	}

private:
	constexpr bool onTerminateCallback() noexcept
	{
		// Atomically count the number of async completed
		auto current = ++counter_;
		// Only trigger the token uppon the first entry in this lambda.
		if (current == 1U)
		{
			token_.trigger();
			token_.detach();
			// Needed to ensure the promise is enqueued after the cancellation token has been triggered.
			current = counter_.fetchAdd(1U, MemoryOrder::acquire);
		}
		// Only at the last function exit the caller is pushed back to the scheduling queue.
		// This makes this design thread safe.
		return (current == sizeof...(Asyncs) + 1U);
	}

private:
	bzd::Atomic<SizeType> counter_{0};
	CancellationToken token_{};
};

} // namespace bzd::coroutine::impl
