#pragma once

#include "cc/components/generic/clock/null/interface.hh"

namespace bzd::platform::generic::clock {

class Null : public bzd::Clock2
{
public:
	template <class Context>
	constexpr explicit Null(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::generic::clock
