#include "cc/components/generic/clock/null/null.hh"

namespace bzd::platform::generic::clock {

ClockTick Null::getTicks() noexcept
{
	return static_cast<ClockTick>(0);
}

ClockTick Null::msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockTick>(time.get());
}

bzd::units::Millisecond Null::ticksToMs(const ClockTick& ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get());
}

} // namespace bzd::platform::generic::clock
