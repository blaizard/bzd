#include "bzd/platform/clock.h"

#include "sdkconfig.h"
#include "xtensa/core-macros.h"

bzd::ClockTickType bzd::platform::getTicks() noexcept
{
	return XTHAL_GET_CCOUNT();
}

bzd::platform::ClockDurationType msToTicks(const bzd::units::Millisecond time) noexcept
{
	return static_cast<bzd::platform::ClockDurationType>(time.get()) * (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000);
}

bzd::units::Millisecond ticksToMs(const bzd::ClockTickType ticks) noexcept
{
	return static_cast<bzd::units::Millisecond>(ticks / (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}
