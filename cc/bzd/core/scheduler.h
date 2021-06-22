#pragma once

#include "cc/bzd/container/impl/non_owning_list.h"
#include "cc/bzd/core/promise.h"
#include "cc/bzd/utility/ignore.h"
#include "cc/bzd/utility/singleton.h"

namespace bzd {
class Scheduler : public bzd::Singleton<Scheduler>
{
public:
	template <class T>
	using AsyncResultType = typename bzd::typeTraits::RemoveReference<T>::ResultType;

public:
	constexpr Scheduler() = default;

	template <class T>
	constexpr void push(bzd::coroutine::impl::coroutine_handle<T>& handle) noexcept
	{
		bzd::ignore = queue_.pushFront(handle.promise());
	}

	template <class T>
	AsyncResultType<T> run(T& async) noexcept
	{
		// Push the handle to the scheduler
		async.attach();

		// Run the scheduler
		run();

		// Return the result.
		return async.getResult().value();
	}

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
		auto promise = queue_.back();
		const auto result = queue_.pop(promise.valueMutable());
		if (!result)
		{
			return bzd::nullopt;
		}
		auto handle = bzd::coroutine::impl::coroutine_handle<bzd::coroutine::interface::Promise>::from_promise(promise.valueMutable());

		// Show the stack usage
		// void* stack = __builtin_frame_address(0);
		// static void* initial_stack = &stack;
		// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPtrType>(initial_stack) -
		// reinterpret_cast<bzd::IntPtrType>(stack)) << std::endl;

		return handle;
	}

private:
	bzd::NonOwningList<bzd::coroutine::interface::Promise> queue_{};
};
} // namespace bzd
