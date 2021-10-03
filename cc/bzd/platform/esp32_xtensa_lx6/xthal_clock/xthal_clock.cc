#include "cc/bzd/platform/esp32_xtensa_lx6/xthal_clock/xthal_clock.hh"

#include "sdkconfig.h"
#include "xtensa/core-macros.h"

namespace bzd::platform::esp32 {

ClockTick XthalClock::getTicks() noexcept
{
	exec();
	return static_cast<ClockTick>(ticks_);
}

ClockTick XthalClock::msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockTick>(static_cast<bzd::UInt64Type>(time.get()) * (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}

bzd::units::Millisecond XthalClock::ticksToMs(const ClockTick& ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get() / (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}

void XthalClock::exec() noexcept
{
	const bzd::UInt32Type counter = XTHAL_GET_CCOUNT();
	const bzd::UInt32Type previous_counter = ticks_ & 0xffffffff;
	bzd::UInt32Type previous_wrapper = (ticks_ >> 32) & 0xffffffff;
	// Means the 32-bit counter wrapped.
	if (previous_counter > counter)
	{
		++previous_wrapper;
	}
	ticks_ = (static_cast<bzd::UInt64Type>(previous_wrapper) << 32) + counter;
}

} // namespace bzd::platform::esp32
