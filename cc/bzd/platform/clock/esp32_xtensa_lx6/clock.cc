#include "bzd/platform/clock.h"

#include "sdkconfig.h"
#include "xtensa/core-macros.h"

bzd::platform::ClockTick bzd::platform::getTicks() noexcept
{
	return static_cast<bzd::platform::ClockTick>(XTHAL_GET_CCOUNT());
}

bzd::platform::ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<bzd::platform::ClockDuration>(static_cast<bzd::UInt64Type>(time.get()) * (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}

bzd::units::Millisecond ticksToMs(const bzd::platform::ClockTick ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks.get() / (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}
