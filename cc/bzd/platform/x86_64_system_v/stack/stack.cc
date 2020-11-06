#include "cc/bzd/platform/stack.h"
#include "cc/bzd/platform/system.h"

namespace bzd::impl {
extern "C" void contextSwitch(void** stack1, void* stack2);
extern "C" void* contextTask();
} // namespace bzd::impl

void bzd::platform::contextSwitch(void** stack1, void* stack2) noexcept
{
	bzd::impl::contextSwitch(stack1, stack2);
}

void* contextTask() noexcept
{
	return bzd::impl::contextTask();
}

void bzd::interface::Stack::reset(const FctPtrType fct, void* context)
{
	auto pStack = last<void*, 16>();
	// pStack[0] is for the return address
	// check why we cannot use -1 or 0
	pStack[-2] = context;
	pStack[-3] = reinterpret_cast<void*>(fct);
	stack_ = reinterpret_cast<UInt8Type*>(pStack - 10);
}