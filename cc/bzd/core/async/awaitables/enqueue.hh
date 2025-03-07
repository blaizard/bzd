#pragma once

#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/utility/apply.hh"

namespace bzd::async::awaitable {

/// Awaitable to enqueue an async object.
template <class... Asyncs>
class Enqueue : public bzd::async::impl::suspend_always
{
public:
	using Self = Enqueue<Asyncs...>;
	template <class T>
	using ResultTypeHelper = typename bzd::typeTraits::RemoveReference<T>::ResultType;

protected:
	template <class... Ts>
	constexpr Enqueue(bzd::async::impl::PromiseBase::OnTerminateCallback callback, Ts&&... asyncs) noexcept :
		callback_{callback}, asyncs_{inPlace, bzd::forward<Ts>(asyncs)...}
	{
	}

	constexpr Enqueue(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Enqueue(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

	constexpr void enqueueAsyncs(bzd::async::impl::PromiseBase& caller,
								 bzd::Optional<CancellationToken&> maybeToken,
								 const Bool parallel) noexcept
	{
		auto& executor{caller.getExecutor()};
		continuation_.emplace(caller);

		// If multi core is enabled, set the context for it.
		auto metadata = caller.getMetadata();
		if (parallel)
		{
			metadata.anyCore();
		}

		// Schedule all asyncs
		const auto schedule = [&](auto& async) {
			auto& executable = async.getExecutable();
			if (maybeToken.hasValue())
			{
				executable.setCancellationToken(maybeToken.valueMutable());
			}
			executable.setConditionalContinuation(callback_);
			executor.schedule(executable, metadata);
		};
		bzd::apply([&](auto&... async) { return (schedule(async), ...); }, asyncs_);
	}

protected:
	bzd::async::impl::PromiseBase::OnTerminateCallback callback_;
	bzd::Tuple<Asyncs...> asyncs_;
	bzd::Optional<bzd::async::impl::PromiseBase&> continuation_{};
};

template <class... Asyncs>
class EnqueueAll : public Enqueue<Asyncs...>
{
public: // Traits.
	using Self = EnqueueAll<Asyncs...>;
	using ResultType = bzd::Tuple<typename Enqueue<Asyncs...>::template ResultTypeHelper<Asyncs>...>;

public: // Constructor.
	template <class... Ts>
	constexpr EnqueueAll(const Bool parallel, Ts&&... asyncs) noexcept :
		Enqueue<Asyncs...>{bzd::async::impl::PromiseBase::OnTerminateCallback::toMember<Self, &Self::onTerminateCallback>(*this),
						   bzd::forward<Ts>(asyncs)...},
		parallel_{parallel}
	{
	}

public: // Coroutine specializations.
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr void await_suspend(bzd::async::impl::coroutine_handle<T> caller) noexcept
	{
		auto& promise = caller.promise();
		this->enqueueAsyncs(promise, promise.getCancellationToken(), parallel_);
	}

	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr ResultType await_resume() noexcept
	{
		return bzd::apply(
			[](auto&... asyncs) -> ResultType {
				// Copy the result of each async into the result type and return it.
				return ResultType{bzd::inPlace, asyncs.moveResultOut()...};
			},
			this->asyncs_);
	}

private:
	constexpr bzd::Optional<bzd::async::impl::PromiseBase&> onTerminateCallback() noexcept
	{
		// Atomically count the number of async completed, and only for the last one,
		// push the caller back into the scheduling queue.
		// This makes this design thread safe.
		if (++counter_ == sizeof...(Asyncs))
		{
			return this->continuation_;
		}
		return bzd::nullopt;
	}

private:
	Bool parallel_;
	bzd::Atomic<Size> counter_{0};
};

template <class... Asyncs>
class EnqueueAny : public Enqueue<Asyncs...>
{
public: // Traits.
	using Self = EnqueueAny<Asyncs...>;
	using ResultType = bzd::Tuple<bzd::Optional<typename Enqueue<Asyncs...>::template ResultTypeHelper<Asyncs>>...>;

public: // Constructor.
	template <class... Ts>
	constexpr EnqueueAny(const Bool parallel, Ts&&... asyncs) noexcept :
		Enqueue<Asyncs...>{bzd::async::impl::PromiseBase::OnTerminateCallback::toMember<Self, &Self::onTerminateCallback>(*this),
						   bzd::forward<Ts>(asyncs)...},
		parallel_{parallel}
	{
	}

public: // Coroutine specializations.
	template <class T>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr void await_suspend(bzd::async::impl::coroutine_handle<T> caller) noexcept
	{
		auto& promise = caller.promise();

		// This defines the cancellation to pass to the executables.
		auto maybeToken = promise.getCancellationToken();
		// Attach the token to the parent if both exists.
		if (maybeToken.hasValue())
		{
			token_.attachTo(maybeToken.valueMutable());
		}

		this->enqueueAsyncs(promise, token_, parallel_);
	}

	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr ResultType await_resume() noexcept
	{
		return bzd::apply(
			[](auto&... asyncs) -> ResultType {
				// By now all asyncs must have been either ready or canceled.
				// Build the result and return it.
				return ResultType{bzd::inPlace, asyncs.moveOptionalResultOut()...};
			},
			this->asyncs_);
	}

private:
	constexpr bzd::Optional<bzd::async::impl::PromiseBase&> onTerminateCallback() noexcept
	{
		// Atomically count the number of async completed
		auto current = ++counter_;
		// Only trigger the token upon the first entry in this lambda.
		if (current == 1U)
		{
			token_.trigger();
			// Needed to ensure the promise is enqueued after the cancellation token has been triggered.
			current = ++counter_;
		}
		// Only at the last function exit the caller is pushed back to the scheduling queue.
		// This makes this design thread safe.
		if (current == sizeof...(Asyncs) + 1u)
		{
			// Detach the token only when all executables returned, this is necessary to ensure all
			// executables can have access to the token to check if it has been triggered or not.
			// This prevents a race with async::suspend(...) for example.
			token_.detach();
			return this->continuation_;
		}
		return bzd::nullopt;
	}

private:
	Bool parallel_;
	bzd::Atomic<Size> counter_{0};
	CancellationToken token_{};
};

} // namespace bzd::async::awaitable
