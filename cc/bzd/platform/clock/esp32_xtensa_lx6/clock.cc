#include "bzd/platform/clock.h"

#include "sdkconfig.h"
#include "xtensa/core-macros.h"

bzd::ClockTickType bzd::platform::getTicks() noexcept
{
	return XTHAL_GET_CCOUNT();
}

bzd::platform::ClockDurationType msToTicks(const bzd::UInt32Type ms) noexcept
{
	return static_cast<bzd::platform::ClockDurationType>(ms) * (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000);
}

bzd::UInt32Type ticksToMs(const bzd::ClockTickType ticks) noexcept
{
	return ticks / (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000);
}
