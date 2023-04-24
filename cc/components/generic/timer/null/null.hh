#pragma once

#include "cc/components/generic/timer/null/interface.hh"

namespace bzd::platform::generic::timer {

class Null : public bzd::Timer
{
public:
	template <class Context>
	constexpr explicit Null(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::generic::timer
