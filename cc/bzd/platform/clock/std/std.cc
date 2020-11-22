#include "bzd/platform/clock.h"

#include <chrono>

bzd::platform::ClockTick bzd::platform::getTicks() noexcept
{
	const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return static_cast<ClockTick>(ticks);
}

bzd::platform::ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<bzd::platform::ClockDuration>(time.get());
}

bzd::units::Millisecond ticksToMs(const bzd::platform::ClockTick ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get());
}
