#include "bzd/platform/stack.h"

#include "bzd/platform/types.h"

namespace bzd::impl {
extern "C" void contextSwitch(bzd::UInt8Type** stack1, bzd::UInt8Type* stack2);
} // namespace bzd::impl

const bzd::platform::interface::Stack::Direction bzd::platform::interface::Stack::direction_ =
	bzd::platform::interface::Stack::Direction::DOWNWARD;

void bzd::platform::interface::Stack::contextSwitch(Stack& stack) noexcept
{
	bzd::impl::contextSwitch(&stack_, stack.stack_);
}

void bzd::platform::interface::Stack::reset(const FctPtrType fct, const TaskPtrType task) noexcept
{
	auto pStack = last<const void*, 16>();
	// pStack[0] is for the return address
	// check why we cannot use -1 or 0
	pStack[-2] = task;
	pStack[-3] = reinterpret_cast<const void*>(fct);
	stack_ = reinterpret_cast<UInt8Type*>(pStack - 10);
}