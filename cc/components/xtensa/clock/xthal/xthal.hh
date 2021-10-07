#pragma once

#include "cc/bzd/core/clock.hh"

namespace bzd::platform::esp32::clock {

class Xthal : public bzd::Clock
{
public:
	ClockTick getTicks() noexcept override;

	ClockTick msToTicks(const bzd::units::Millisecond time) noexcept override;

	bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept override;

	void exec() noexcept;

private:
	bzd::UInt64Type ticks_{0};
};

} // namespace bzd::platform::esp32::clock
