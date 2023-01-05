#pragma once

#include "cc/bzd/core/clock.hh"

namespace bzd::platform::std::clock {

class Steady : public bzd::Clock
{
public:
	template <class Config>
	constexpr Steady(const Config&) noexcept
	{
	}

	ClockTick getTicks() noexcept override;

	ClockTick msToTicks(const bzd::units::Millisecond time) noexcept override;

	bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept override;
};

} // namespace bzd::platform::std::clock
