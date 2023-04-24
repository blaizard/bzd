#pragma once

#include "cc/components/std/clock/steady/interface.hh"

namespace bzd::platform::std::clock {

class Steady : public bzd::Timer
{
public:
	template <class Context>
	constexpr Steady(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::std::clock
