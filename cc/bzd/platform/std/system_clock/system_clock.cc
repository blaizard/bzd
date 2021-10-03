#include "cc/bzd/platform/std/system_clock/system_clock.hh"

#include <chrono>
#include <iostream>
namespace bzd::platform::std {

ClockTick SystemClock::getTicks() noexcept
{
	const auto ticks =
		::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::system_clock::now().time_since_epoch()).count();
	return static_cast<ClockTick>(ticks);
}

ClockTick SystemClock::msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockTick>(time.get());
}

bzd::units::Millisecond SystemClock::ticksToMs(const ClockTick& ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get());
}

} // namespace bzd::platform::std
