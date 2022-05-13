#pragma once

#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/utility/apply.hh"

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
	constexpr Enqueue(Executable::OnTerminateCallback callback, Ts&&... asyncs) noexcept :
		callback_{callback}, asyncs_{inPlace, bzd::forward<Ts>(asyncs)...}
	{
	}

	constexpr Enqueue(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Enqueue(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

	constexpr void enqueueAsyncs(Executable& caller, bzd::Optional<CancellationToken&> maybeToken) noexcept
	{
		auto& executor{caller.getExecutor()};
		continuation_.emplace(caller);
		constexprForContainerInc(asyncs_, [&](auto& async) {
			auto& executable = async.getExecutable();
			if (maybeToken.hasValue())
			{
				executable.setCancellationToken(maybeToken.valueMutable());
			}
			executable.setConditionalContinuation(Executable::OnTerminateCallback::toMember<Self, &Self::onTerminateCallback>(*this));
			executor.schedule(executable, caller.getType());
		});
	}

	constexpr bzd::Optional<Executable&> onTerminateCallback() noexcept
	{
		switch (continuation_->getType())
		{
		case bzd::ExecutableMetadata::Type::active:
			continuation_->getExecutor().terminateActive();
			break;
		case bzd::ExecutableMetadata::Type::service:
			break;
		case bzd::ExecutableMetadata::Type::unset:
			[[fallthrough]];
		default:
			bzd::assert::unreachable();
		}
		return callback_();
	}

protected:
	Executable::OnTerminateCallback callback_;
	bzd::Tuple<Asyncs...> asyncs_;
	bzd::Optional<Executable&> continuation_{};
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
		Enqueue<Asyncs...>{Executable::OnTerminateCallback::toMember<Self, &Self::onTerminateCallback>(*this), bzd::forward<Ts>(asyncs)...}
	{
	}

public: // Coroutine specializations.
	template <class T>
	constexpr void await_suspend(bzd::coroutine::impl::coroutine_handle<T> caller) noexcept
	{
		auto& promise = caller.promise();
		this->enqueueAsyncs(promise, promise.getCancellationToken());
	}

	[[nodiscard]] constexpr ResultType await_resume() noexcept
	{
		return bzd::apply(
			[](auto&... asyncs) -> ResultType {
				// Copy the result of each async into the result type and return it.
				return ResultType{bzd::inPlace, asyncs.await_resume()...};
			},
			this->asyncs_);
	}

private:
	constexpr bzd::Optional<Executable&> onTerminateCallback() noexcept
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
		Enqueue<Asyncs...>{Executable::OnTerminateCallback::toMember<Self, &Self::onTerminateCallback>(*this), bzd::forward<Ts>(asyncs)...}
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

		this->enqueueAsyncs(promise, token_);
	}

	[[nodiscard]] constexpr ResultType await_resume() noexcept
	{
		return bzd::apply(
			[](auto&... asyncs) -> ResultType {
				// By now all asyncs must have been either ready or canceled.
				// Build the result and return it.
				return ResultType{bzd::inPlace, asyncs.moveResultOut()...};
			},
			this->asyncs_);
	}

private:
	constexpr bzd::Optional<Executable&> onTerminateCallback() noexcept
	{
		// Atomically count the number of async completed
		auto current = ++counter_;
		// Only trigger the token uppon the first entry in this lambda.
		if (current == 1U)
		{
			token_.trigger();
			token_.detach();
			// Needed to ensure the promise is enqueued after the cancellation token has been triggered.
			current = ++counter_;
		}
		// Only at the last function exit the caller is pushed back to the scheduling queue.
		// This makes this design thread safe.
		if (current == sizeof...(Asyncs) + 1U)
		{
			return this->continuation_;
		}
		return bzd::nullopt;
	}

private:
	bzd::Atomic<SizeType> counter_{0};
	CancellationToken token_{};
};

} // namespace bzd::coroutine::impl
