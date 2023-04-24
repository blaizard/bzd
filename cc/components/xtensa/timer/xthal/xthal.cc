#include "cc/components/xtensa/timer/xthal/xthal.hh"

#include "sdkconfig.h"
#include "xtensa/core-macros.h"

namespace bzd::platform::esp32::timer {

bzd::Result<bzd::units::Millisecond, bzd::Error> Xthal::getTime() noexcept
{
	exec().sync();
	return static_cast<bzd::units::Millisecond>(ticks_.get() / (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000));
}

bzd::Async<> Xthal::exec() noexcept
{
	const bzd::UInt32 counter = XTHAL_GET_CCOUNT();
	const bzd::UInt32 previous_counter = ticks_ & 0xffffffff;
	bzd::UInt32 previous_wrapper = (ticks_ >> 32) & 0xffffffff;
	// Means the 32-bit counter wrapped.
	if (previous_counter > counter)
	{
		++previous_wrapper;
	}
	ticks_ = (static_cast<bzd::UInt64>(previous_wrapper) << 32) + counter;
	co_return {};
}

} // namespace bzd::platform::esp32::timer
