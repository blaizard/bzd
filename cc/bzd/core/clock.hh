#pragma once

#include "cc/bzd/container/named_type.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

class ClockTick : public bzd::NamedType<bzd::UInt64Type, struct ClockTickTag, bzd::Arithmetic>
{
public:
	using bzd::NamedType<bzd::UInt64Type, struct ClockTickTag, bzd::Arithmetic>::NamedType;
};

class Clock
{
public:
	virtual ClockTick getTicks() noexcept = 0;
	virtual ClockTick msToTicks(const bzd::units::Millisecond time) noexcept = 0;
	virtual bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept = 0;
	bzd::units::Millisecond getMs() noexcept { return ticksToMs(getTicks()); }
};

} // namespace bzd
