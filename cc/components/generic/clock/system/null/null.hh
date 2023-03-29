#pragma once

#include "cc/components/generic/clock/system/null/interface.hh"

namespace bzd::platform::generic::clock::system {

class Null : public bzd::SystemClock
{
public:
	template <class Context>
	constexpr explicit Null(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept override;
};

} // namespace bzd::platform::generic::clock::system
