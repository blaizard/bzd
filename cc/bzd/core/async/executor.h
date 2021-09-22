#pragma once

#include "cc/bzd/container/impl/non_owning_list.h"
#include "cc/bzd/core/async/promise.h"
#include "cc/bzd/utility/ignore.h"

#include <iostream>

namespace bzd {
class Executor
{
public:
	class Executable : public bzd::NonOwningListElement<true>
	{
	public:
		constexpr void enqueue() noexcept
		{
			bzd::assert::isTrue(executor_);
			executor_->push(*this);
		}

		bzd::Executor* executor_{nullptr};
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
		auto exectuable = queue_.back();
		if (exectuable)
		{
			const auto result = queue_.pop(exectuable.valueMutable());
			if (result)
			{
				auto handle = bzd::coroutine::impl::coroutine_handle<bzd::Executor::Executable>::from_promise(exectuable.valueMutable());

				// Show the stack usage
				// void* stack = __builtin_frame_address(0);
				// static void* initial_stack = &stack;
				// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPtrType>(initial_stack) -
				// reinterpret_cast<bzd::IntPtrType>(stack)) << std::endl;

				return handle;
			}
		}

		return bzd::nullopt;
	}

private:
	bzd::NonOwningList<bzd::Executor::Executable> queue_{};
};
} // namespace bzd
