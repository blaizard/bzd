#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/executor.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/utility/constexpr_for.hh"
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
public: // Traits.
	using OnTerminateCallback = bzd::FunctionRef<bzd::Optional<Executable&>(void)>;

public:
	constexpr explicit Executable(bzd::coroutine::impl::coroutine_handle<> handle) noexcept : handle_{handle} {}

	/// Called by the scheduler to resume an executable.
	void resume(bzd::ExecutorContext<Executable>& context) noexcept
	{
		context_ = &context;
		handle_.resume();
	}

	/// Called by the scheduler when an executable is detected as being canceled.
	constexpr void cancel(bzd::ExecutorContext<Executable>& context) noexcept
	{
		auto executable = this;

		// Unroll the callstaxck and stop to the first callback and execute it.
		do
		{
			executable = executable->getContinuation();
		} while (executable && executable->isCanceled());

		if (executable)
		{
			// Enqueue the continuation for later use, it will be scheduled
			// according to the executor policy.
			context.enqueue(*executable);
		}
	}

	constexpr Executable* getContinuation() noexcept
	{
		Executable* executable{nullptr};
		continuation_.match([](bzd::monostate) {},
							[&](Executable* e) { executable = e; },
							[&](OnTerminateCallback callback) {
								auto result = callback();
								if (result)
								{
									executable = &(result.valueMutable());
								}
							});

		return executable;
	}

	/// Enqueue a new executable after the execution of the current executable.
	/// This is needed for thread safe scheduling, to avoid executing the continuation
	/// before the current coroutine state is not completed.
	constexpr void enqueueAfterExecution(Executable& executable) noexcept
	{
		bzd::assert::isTrue(context_);
		context_->enqueue(executable);
	}

	constexpr void setContinuation(Executable& continuation) noexcept { continuation_.emplace<Executable*>(&continuation); }

	constexpr void setConditionalContinuation(OnTerminateCallback onTerminate) noexcept
	{
		continuation_.emplace<OnTerminateCallback>(onTerminate);
	}

private:
	bzd::coroutine::impl::coroutine_handle<> handle_;
	bzd::ExecutorContext<Executable>* context_{nullptr};
	bzd::ExecutorContext<Executable>::Continuation continuation_{};
};

using Executor = bzd::Executor<Executable>;

template <class T>
class Promise : public Executable
{
private:
	using Self = Promise<T>;

private:
	struct FinalAwaiter
	{
		constexpr FinalAwaiter(const bzd::BoolType propagate) noexcept : propagate_{propagate} {}

		constexpr bool await_ready() noexcept { return false; }

		constexpr bzd::coroutine::impl::coroutine_handle<> await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
		{
			// TODO: Check if this is an error, if so propagate it down to the layers until a catch block is detected.
			if (propagate_ && handle.promise().hasError())
			{
				::std::cout << "ERROR!!!!" << ::std::endl;
			}

			// Enqueuing this coroutine into the executor is important for thread
			// safety reasons, if we return the coroutine handle directly here, we
			// might have a case where the same coroutine executes multiple times on
			// different threads.
			auto maybeContinuation = handle.promise().getContinuation();
			if (maybeContinuation)
			{
				handle.promise().enqueueAfterExecution(*maybeContinuation);
			}

			return bzd::coroutine::impl::noop_coroutine();
		}

		constexpr void await_resume() noexcept {}

		bzd::BoolType propagate_{};
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

	constexpr FinalAwaiter final_suspend() noexcept { return FinalAwaiter{propagate_}; }

	constexpr void unhandled_exception() noexcept { bzd::assert::unreachable(); }

	constexpr void setPropagate() noexcept { propagate_ = true; }

public: // Memory allocation
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

	bzd::BoolType propagate_{false};
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
