#include "cc/bzd/platform/clock.hh"

#include <chrono>

namespace bzd::platform {

ClockTick getTicks() noexcept
{
	const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	return static_cast<ClockTick>(ticks);
}

ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockDuration>(time.get());
}

bzd::units::Millisecond ticksToMs(const ClockTick ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get());
}

} // namespace bzd::platform
