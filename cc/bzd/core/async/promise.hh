#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/core/async/executor.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/utility/constexpr_for.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd::coroutine::impl {

/// Executable type for coroutines.
class Executable : public bzd::interface::Executable<Executable>
{
public: // Traits.
	using OnTerminateCallback = bzd::FunctionRef<bzd::Optional<Executable&>(void)>;
	using SetErrorCallback = bzd::FunctionRef<void(bzd::Error&&)>;

public:
	constexpr explicit Executable(bzd::coroutine::impl::coroutine_handle<> handle, SetErrorCallback&& callback) noexcept :
		handle_{handle}, setError_{bzd::move(callback)}
	{
	}

	/// Called by the scheduler to resume an executable.
	void resume(bzd::ExecutorContext<Executable>& context) noexcept
	{
		context_ = &context;
		handle_.resume();
	}

	/// Called by the scheduler when an executable is detected as being canceled.
	constexpr void cancel(bzd::ExecutorContext<Executable>& context) noexcept
	{
		bzd::Optional<Executable&> executable{*this};

		// Unroll all the direct continuation from the callstackk.
		while (executable->continuation_.is<Executable*>())
		{
			executable.emplace(*(executable->continuation_.get<Executable*>()));
		}

		// All cancellation must terminate with a callback, if this fails it means that
		// this cancellation async does not have a parent which should never happen.
		bzd::assert::isTrue(executable->continuation_.is<OnTerminateCallback>());

		context_ = &context;
		thenEnqueueContinuation();
	}

	/// Enqueue a new executable after the execution of the current executable.
	/// This is needed for thread safe scheduling, to avoid executing the continuation
	/// before the current coroutine state is not completed.
	constexpr void thenEnqueueContinuation() noexcept
	{
		bzd::assert::isTrue(context_);
		context_->setContinuation(bzd::move(continuation_));
	}

	constexpr void thenEnqueueExecutable(Executable& executable) noexcept
	{
		bzd::assert::isTrue(context_);
		context_->setContinuation(bzd::ExecutorContext<Executable>::Continuation{&executable});
	}

	constexpr void setContinuation(Executable& continuation) noexcept { continuation_.emplace<Executable*>(&continuation); }

	constexpr void setConditionalContinuation(OnTerminateCallback onTerminate) noexcept
	{
		continuation_.emplace<OnTerminateCallback>(onTerminate);
	}

	constexpr void setPropagate() noexcept { setError_.reset(); }

	constexpr bool mustPropagateError() const noexcept { return !setError_.hasValue(); }

	/// Propagate the error to the first parent accepting errors and set the continuation
	/// of this executable to this parent.
	constexpr void propagateError(bzd::Error&& error) noexcept
	{
		// Look for the first caller that does not have the propagate flag and resume.
		auto executable{this};
		do
		{
			auto& continuation = executable->continuation_;
			bzd::assert::isTrue(continuation.template is<Executable*>() && continuation.template get<Executable*>(),
								"Cannot propagate error on this type of async");
			executable = continuation.template get<Executable*>();
		} while (executable->mustPropagateError());
		continuation_ = bzd::move(executable->continuation_);

		// Set the error here
		executable->setError_.value()(bzd::move(error));
	}

private:
	bzd::coroutine::impl::coroutine_handle<> handle_;
	bzd::Optional<SetErrorCallback> setError_;
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
		constexpr bool await_ready() noexcept { return false; }

		constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<T> handle) noexcept
		{
			auto& promise = handle.promise();

			// Propagate the error if needed.
			if (auto error = promise.hasErrorToPropagate(); error.hasValue())
			{
				promise.propagateError(bzd::move(error.valueMutable()));
			}

			// Enqueuing the continuation with the executor is important for thread
			// safety reasons, if we return the coroutine handle directly here, we
			// might have a case where the same coroutine executes multiple times on
			// different threads.
			promise.thenEnqueueContinuation();

			return true;
		}

		constexpr void await_resume() noexcept {}
	};

public:
	constexpr Promise(SetErrorCallback&& callback) noexcept :
		Executable{bzd::coroutine::impl::coroutine_handle<T>::from_promise(static_cast<T&>(*this)), bzd::move(callback)}
	{
	}

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

	constexpr FinalAwaiter final_suspend() noexcept { return {}; }

	constexpr void unhandled_exception() noexcept { bzd::assert::unreachable(); }

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
};

} // namespace bzd::coroutine::impl

namespace bzd::coroutine {
template <class T>
class Promise : public impl::Promise<Promise<T>>
{
public:
	using Self = Promise<T>;
	using ResultType = T;
	using impl::Promise<Promise<T>>::Promise;
	static constexpr BoolType resultTypeIsResult = concepts::sameTemplate<ResultType, bzd::Result>;
	static constexpr BoolType resultTypeIsTuple = concepts::sameTemplate<ResultType, bzd::Tuple>;

	constexpr Promise() noexcept : impl::Promise<Self>{impl::Executable::SetErrorCallback::toMember<Self, &Self::setError>(*this)} {}

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

	constexpr bool isReady() const noexcept { return result_.hasValue(); }

	constexpr bzd::Optional<T> moveResultOut() noexcept { return bzd::move(result_); }

	/// Check if the result contains an error.
	bzd::Optional<bzd::Error> hasErrorToPropagate() noexcept
	{
		if (this->mustPropagateError())
		{
			if constexpr (resultTypeIsResult)
			{
				if (result_.hasValue() && result_.value().hasError())
				{
					return bzd::move(result_.valueMutable().errorMutable());
				}
			}
			else if constexpr (resultTypeIsTuple)
			{
			}
			else
			{
				bzd::assert::isTrue(false, "This type of async is not supported");
			}
		}
		// if constexpr (bzd::Tuple<bzd::Result<...>>) {}
		return bzd::nullopt;
	}

	constexpr void setError([[maybe_unused]] bzd::Error&& error) noexcept
	{
		if constexpr (resultTypeIsResult)
		{
			result_ = bzd::error(bzd::move(error));
		}
		else
		{
			bzd::assert::isTrue(false, "This type of async is not supported");
		}
	}

private:
	bzd::Optional<T> result_{};
};

} // namespace bzd::coroutine
