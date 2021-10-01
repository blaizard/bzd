#pragma once

#include "cc/bzd/container/named_type.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

class ClockDuration : public bzd::NamedType<bzd::UInt64Type, struct ClockDurationTag, bzd::Arithmetic>
{
public:
	using bzd::NamedType<bzd::UInt64Type, struct ClockDurationTag, bzd::Arithmetic>::NamedType;
	struct Details
	{
		bzd::UInt32Type wrappingCounter;
		bzd::UInt32Type ticks;
	};

public:
	constexpr Details getDetails() const noexcept
	{
		return Details{static_cast<bzd::UInt32Type>(this->get() >> 32), static_cast<bzd::UInt32Type>(this->get())};
	}

	constexpr void setFromDetails(const Details& details) noexcept
	{
		this->get() = (static_cast<bzd::UInt64Type>(details.wrappingCounter) << 32) + details.ticks;
	}
};

class ClockTick : public bzd::NamedType<bzd::UInt32Type, struct ClockTickTag, bzd::Arithmetic>
{
public:
	using bzd::NamedType<bzd::UInt32Type, struct ClockTickTag, bzd::Arithmetic>::NamedType;

public:
	constexpr ClockDuration toDuration() const { return ClockDuration{this->get()}; }
};

class Clock
{
public:
    virtual ClockTick getTicks() noexcept = 0;
    virtual ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept = 0;
    virtual bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept = 0;
};

}
