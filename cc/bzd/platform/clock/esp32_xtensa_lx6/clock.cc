#include "bzd/platform/clock.h"

#include "sdkconfig.h"
#include "xtensa/core-macros.h"

namespace bzd::platform {

ClockTick getTicks() noexcept
{
	return static_cast<ClockTick>(XTHAL_GET_CCOUNT());
}

ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<ClockDuration>(static_cast<bzd::UInt64Type>(time.get()) * (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}

bzd::units::Millisecond ticksToMs(const ClockTick ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get() / (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}

} // namespace bzd::platform