#include "cc/components/std/clock/steady/steady.hh"

#include <chrono>

namespace bzd::platform::std::clock {

ClockTick Steady::getTicks() noexcept
{
	const auto ticks =
		::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch()).count();
	return static_cast<ClockTick>(ticks);
}

ClockTick Steady::msToTicks(const bzd::units::Millisecond time) noexcept { return static_cast<ClockTick>(time.get()); }

bzd::units::Millisecond Steady::ticksToMs(const ClockTick& ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get()); // NOLINT(bugprone-narrowing-conversions)
}

} // namespace bzd::platform::std::clock
