#include "cc/bzd/platform/stack.h"

const bzd::platform::interface::Stack::Direction bzd::platform::interface::Stack::direction_ =
	bzd::platform::interface::Stack::Direction::DOWNWARD;

void bzd::platform::interface::StackUser::reset(const FctPtrType fct) noexcept
{
	auto pStack = last<bzd::UInt64Type, 16>();
	// The function address
	pStack[0] = reinterpret_cast<bzd::UInt64Type>(fct);
	// MMX & x87, default MXCSR value at reset is 1F80H
	pStack[-7] = (static_cast<bzd::UInt64Type>(0x0) << 32) | 0x1f80;
	// Set the stack pointer
	stack_ = static_cast<bzd::PtrType>(pStack - 7);
}
