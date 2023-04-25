#pragma once

#include "cc/components/generic/clock/null/interface.hh"

namespace bzd::components::generic::clock {

class Null : public bzd::Clock
{
public:
	template <class Context>
	constexpr explicit Null(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::components::generic::clock
