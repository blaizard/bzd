#pragma once

#include "cc/components/std/clock/system/interface.hh"

namespace bzd::platform::std::clock {

class System : public bzd::Clock2
{
public:
	template <class Context>
	constexpr System(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::std::clock
