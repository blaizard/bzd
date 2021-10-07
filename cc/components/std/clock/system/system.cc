#include "cc/components/std/clock/system/system.hh"

#include <chrono>

namespace bzd::platform::std::clock {

ClockTick System::getTicks() noexcept
{
	const auto ticks =
		::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::system_clock::now().time_since_epoch()).count();
	return static_cast<ClockTick>(ticks);
}

ClockTick System::msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockTick>(time.get());
}

bzd::units::Millisecond System::ticksToMs(const ClockTick& ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get());
}

} // namespace bzd::platform::std::clock
