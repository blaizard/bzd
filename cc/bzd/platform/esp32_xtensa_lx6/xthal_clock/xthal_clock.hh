#pragma once

#include "cc/bzd/core/clock.hh"

namespace bzd::platform::esp32 {

class XthalClock : public bzd::Clock
{
public:
	ClockTick getTicks() noexcept override;

	ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept override;

	bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept override;
};

}
