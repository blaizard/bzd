#include "bzd/platform/clock.h"

#include <chrono>

bzd::ClockTickType bzd::platform::getTicks() noexcept
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bzd::platform::ClockDurationType msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<bzd::platform::ClockDurationType>(time.get());
}

bzd::units::Millisecond ticksToMs(const bzd::ClockTickType ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks);
}
