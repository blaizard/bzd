#include "bzd/platform/clock.h"

#include <chrono>

bzd::ClockTickType bzd::platform::getTicks() noexcept
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bzd::platform::ClockDurationType msToTicks(const bzd::UInt32Type ms) noexcept
{
	return static_cast<bzd::platform::ClockDurationType>(ms);
}

bzd::UInt32Type ticksToMs(const bzd::ClockTickType ticks) noexcept
{
	return ticks;
}
