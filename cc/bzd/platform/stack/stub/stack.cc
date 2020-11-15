#include "bzd/platform/stack.h"

const bzd::platform::interface::Stack::Direction bzd::platform::interface::Stack::direction_ =
	bzd::platform::interface::Stack::Direction::DOWNWARD;

extern "C" {
void stackSwitch(bzd::PtrType*, bzd::PtrType*) {}
}

void bzd::platform::interface::StackUser::reset(const FctPtrType) noexcept {}
