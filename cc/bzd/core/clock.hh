#pragma once

#include "cc/bzd/container/named_type.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

using ClockTick = bzd::NamedType<bzd::UInt64, struct ClockTickTag, bzd::Arithmetic>;

class Clock
{
public:
	virtual ClockTick getTicks() noexcept = 0;
	virtual ClockTick msToTicks(const bzd::units::Millisecond time) noexcept = 0;
	virtual bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept = 0;
	virtual bzd::Async<> delay(const bzd::units::Millisecond time) noexcept
	{
		const auto targetTicks = getTicks() + msToTicks(time);
		do
		{
			co_await bzd::async::yield();
			// Check if the duration is reached
		} while (getTicks() < targetTicks);

		co_return {};
	}

	/// Return the number of milliseconds since the Unix epoch (00:00:00 UTC on 1 January 1970).
	bzd::units::Millisecond getMs() noexcept { return ticksToMs(getTicks()); }
};

} // namespace bzd
