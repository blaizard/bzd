#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/executor.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/utility/source_location.hh"

// Forward declaration
namespace bzd::impl {
template <class T>
class Async;
}

namespace bzd::coroutine::impl {

/// Executable type for coroutines.
class Executable : public bzd::interface::Executable<Executable>
{
public:
	constexpr explicit Executable(bzd::coroutine::impl::coroutine_handle<> handle) noexcept : handle_{handle} {}

	/// Called by the scheduler to resume an executable.
	void resume() noexcept { handle_.resume(); }

	/// Called by the scheduler when an executable is detected as being canceled.
	constexpr void cancel() noexcept
	{
		auto executable = this;
		// Look for the parent cancelled async and enqueue its continuation.
		while (executable->caller_ && executable->caller_->isCanceledOrTriggered())
		{
			executable = executable->caller_;
		}
		executable->enqueueContinuationIfRelevant();
	}

	/// Enqueue the continuation of the async after calling the terminate callback if any.
	constexpr void enqueueContinuationIfRelevant() noexcept
	{
		auto continuation = caller_;

		// Call the termination callback which decides if a continuation should be enqueued.
		if (onTerminateCallback_)
		{
			continuation = ((onTerminateCallback_.value())()) ? continuation : nullptr;
		}

		if (continuation)
		{
			// Enqueue the continuation for later use, it will be scheduled
			// according to the executor policy.
			continuation->enqueue();
		}
	}

	bzd::coroutine::impl::coroutine_handle<> handle_;
	Executable* caller_{nullptr};
	bzd::Optional<bzd::FunctionRef<bool(void)>> onTerminateCallback_{};
};

using Executor = bzd::Executor<Executable>;

/// Awaitable to enqueue an async object.
template <class... Asyncs>
struct Enqueue : public bzd::coroutine::impl::suspend_always
{
	template <class Callback>
	constexpr Enqueue(Callback& callback, Asyncs&... asyncs) noexcept : exectuables_{inPlace, &asyncs.getExecutable()...}
	{
		for (auto& executable : exectuables_)
		{
			executable->onTerminateCallback_.emplace(callback);
		}
	}

	/// Enqueuing the executable in await_suspend is important, as it is called just after
	/// the state of the resume point has been updated. Doing this ensures that concurrent
	/// thread they execute the executable will return at this resume point and not before.
	constexpr void await_suspend(bzd::coroutine::impl::coroutine_handle<>) const noexcept
	{
		for (auto* exectuable : exectuables_)
		{
			exectuable->enqueue();
		}
	}

	bzd::Array<Executable*, sizeof...(Asyncs)> exectuables_;
};

/// Awaitable to yield the current execution.
struct GetExecutable : public bzd::coroutine::impl::suspend_never
{
	constexpr auto await_resume() noexcept { return executable_; }
	Executable* executable_;
};

/// Awaitable to yield the current execution.
struct GetExecutor : public bzd::coroutine::impl::suspend_never
{
	constexpr auto await_resume() noexcept { return executor_; }
	Executor* executor_;
};

/// Awaitable to yield the current execution.
struct Yield : public bzd::coroutine::impl::suspend_always
{
	template <class T>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
	{
		handle.promise().getExecutor().push(handle.promise());
		return true;
	}
};

template <class T>
class Promise : public Executable
{
private:
	using Self = Promise<T>;

private:
	struct FinalAwaiter
	{
		constexpr bool await_ready() noexcept { return false; }

		constexpr bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
		{
			// TODO: Check if this is an error, if so propagate it down to the layers until a catch block is detected.
			if (handle.promise().hasError())
			{
			}

			// Enqueuing this coroutine into the executor is important for thread
			// safety reasons, if we return the coroutine handle directly here, we
			// might have a case where the same coroutine executes multiple times on
			// different threads.
			handle.promise().enqueueContinuationIfRelevant();

			return bzd::coroutine::impl::noop_coroutine();
		}

		constexpr void await_resume() noexcept {}
	};

public:
	constexpr Promise() noexcept : Executable{bzd::coroutine::impl::coroutine_handle<T>::from_promise(static_cast<T&>(*this))} {}

	constexpr Promise(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Promise(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;
	~Promise() noexcept = default;

	constexpr bzd::coroutine::impl::suspend_always initial_suspend(/*SourceLocation source = SourceLocation::current()*/) noexcept
	{
		//::std::cout << "{" << source.getFile() << ":" << source.getLine() << "    " << source.getFunction() << "}" << ::std::endl;

		return {};
	}

	constexpr FinalAwaiter final_suspend() noexcept { return FinalAwaiter{}; }

	constexpr void unhandled_exception() noexcept { bzd::assert::unreachable(); }

public: // Await transform specializations
	template <class... Args>
	constexpr auto await_transform(bzd::coroutine::impl::Enqueue<Args...>&& awaitable) noexcept
	{
		for (auto* exectuable : awaitable.exectuables_)
		{
			exectuable->caller_ = this;
			exectuable->setExecutor(getExecutor());
		}
		return bzd::move(awaitable);
	}

	constexpr auto&& await_transform(bzd::coroutine::impl::Yield&& awaitable) noexcept { return bzd::move(awaitable); }
	constexpr auto&& await_transform(bzd::coroutine::impl::GetExecutable&& awaitable) noexcept
	{
		awaitable.executable_ = this;
		return bzd::move(awaitable);
	}
	constexpr auto&& await_transform(bzd::coroutine::impl::GetExecutor&& awaitable) noexcept
	{
		awaitable.executor_ = &getExecutor();
		return bzd::move(awaitable);
	}

	template <class Async>
	constexpr auto&& await_transform(Async&& async) noexcept
	{
		// Associate the executor of the caller with the new coroutine.
		// NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
		propagateContextTo(async.getExecutable());
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

	struct Empty
	{
	};
	/// Overload to support `co_return {};`.
	constexpr void return_value(Empty) noexcept { result_.emplace(bzd::nullresult); }

	/// Check if the result contains an error.
	constexpr bool hasError() noexcept
	{
		if constexpr (concepts::sameTemplate<ResultType, bzd::Result>)
		{
			return (result_.hasValue() && result_.value().hasError());
		}
		// if constexpr (bzd::Tuple<bzd::Result<...>>) {}
		return false;
	}

private:
	template <class U>
	friend class ::bzd::impl::Async;

	bzd::Optional<T> result_{};
};

} // namespace bzd::coroutine
