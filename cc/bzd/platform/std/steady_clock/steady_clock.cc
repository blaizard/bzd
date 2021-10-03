#include "cc/bzd/platform/std/steady_clock/steady_clock.hh"

#include <chrono>

namespace bzd::platform::std {

ClockTick SteadyClock::getTicks() noexcept
{
	const auto ticks =
		::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch()).count();
	return static_cast<ClockTick>(ticks);
}

ClockTick SteadyClock::msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockTick>(time.get());
}

bzd::units::Millisecond SteadyClock::ticksToMs(const ClockTick& ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get());
}

} // namespace bzd::platform::std
