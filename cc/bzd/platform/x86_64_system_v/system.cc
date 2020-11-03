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
