#pragma once

#include "cc/bzd/container/impl/non_owning_list.h"
#include "cc/bzd/core/promise.h"
#include "cc/bzd/utility/ignore.h"
#include "cc/bzd/utility/singleton.h"

namespace bzd {
class Scheduler : public bzd::Singleton<Scheduler>
{
public:
	constexpr Scheduler() = default;

	template <class T>
	constexpr void push(bzd::coroutine::impl::coroutine_handle<T>& handle) noexcept
	{
		bzd::ignore = queue_.pushFront(handle.promise());
	}

	bzd::coroutine::impl::coroutine_handle<> pop() noexcept
	{
		while (queue_.empty())
		{
			// Call wait callback
		}
		auto promise = queue_.back();
		bzd::ignore = queue_.pop(promise.valueMutable());
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
