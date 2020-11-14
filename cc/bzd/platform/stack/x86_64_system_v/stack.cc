#include "bzd/platform/stack.h"

const bzd::platform::interface::Stack::Direction bzd::platform::interface::Stack::direction_ =
	bzd::platform::interface::Stack::Direction::DOWNWARD;

void bzd::platform::interface::StackUser::reset(const FctPtrType fct) noexcept
{
	auto pStack = last<bzd::UInt64Type, 16>();
	// pStack[0] is for the return address
	// check why we cannot use -1 or 0
	//pStack[-2] = nullptr; //<- needs to be removed
	pStack[-3] = reinterpret_cast<bzd::UInt64Type>(fct);
	pStack[-10] = (static_cast<bzd::UInt64Type>(0x0) << 32) | 0x1f80; //<- default MXCSR value at reset is 1F80H.
	stack_ = reinterpret_cast<UInt8Type*>(pStack - 10);
}
