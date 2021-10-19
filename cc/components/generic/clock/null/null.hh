#pragma once

#include "cc/bzd/core/clock.hh"

namespace bzd::platform::generic::clock {

class Null : public bzd::Clock
{
public:
	ClockTick getTicks() noexcept override;

	ClockTick msToTicks(const bzd::units::Millisecond time) noexcept override;

	bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept override;
};

} // namespace bzd::platform::generic::clock