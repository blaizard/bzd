#pragma once

#include "cc/bzd/container/impl/non_owning_list.hh"
#include "cc/bzd/core/async/promise.hh"
#include "cc/bzd/utility/ignore.hh"

#include <iostream>

namespace bzd {
class Executor
{
public:
	class Executable : public bzd::NonOwningListElement<true>
	{
	public:
		constexpr explicit Executable(bzd::coroutine::impl::coroutine_handle<> handle) noexcept : handle_{handle} {}

		constexpr void enqueue() noexcept
		{
			bzd::assert::isTrue(executor_);
			executor_->push(*this);
		}

		bzd::coroutine::impl::coroutine_handle<> handle_;
		bzd::coroutine::impl::coroutine_handle<> caller_{nullptr};
		bzd::Executor* executor_{nullptr};
		bzd::Optional<bzd::FunctionView<void(Executor::Executable&)>> onTerminateCallback_{};
	};

public:
	template <class T>
	using AsyncResultType = typename bzd::typeTraits::RemoveReference<T>::ResultType;

public:
	constexpr Executor() = default;

	constexpr void push(Executable& exectuable) noexcept { bzd::ignore = queue_.pushFront(exectuable); }

	void run()
	{
		// Loop until there are still elements
		while (!queue_.empty())
		{
			// Drain remaining handles
			auto handle = pop();
			if (handle.hasValue())
			{
				handle->resume();
			}
		}
	}

private:
	bzd::Optional<bzd::coroutine::impl::coroutine_handle<>> pop() noexcept
	{
		auto executable = queue_.back();
		if (executable)
		{
			const auto result = queue_.pop(executable.valueMutable());
			if (result)
			{
				// Show the stack usage
				// void* stack = __builtin_frame_address(0);
				// static void* initial_stack = &stack;
				// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPtrType>(initial_stack) -
				// reinterpret_cast<bzd::IntPtrType>(stack)) << std::endl;

				return executable->handle_;
			}
		}

		return bzd::nullopt;
	}

private:
	bzd::NonOwningList<bzd::Executor::Executable> queue_{};
};
} // namespace bzd
