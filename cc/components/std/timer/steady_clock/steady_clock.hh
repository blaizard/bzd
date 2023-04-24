#pragma once

#include "cc/components/std/timer/steady_clock/interface.hh"

namespace bzd::platform::std::timer {

class SteadyClock : public bzd::Timer
{
public:
	template <class Context>
	constexpr SteadyClock(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::std::timer
