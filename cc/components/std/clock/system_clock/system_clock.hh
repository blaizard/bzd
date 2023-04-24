#pragma once

#include "cc/components/std/clock/system_clock/interface.hh"

namespace bzd::platform::std::clock {

class SystemClock : public bzd::Clock
{
public:
	template <class Context>
	constexpr SystemClock(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::std::clock
