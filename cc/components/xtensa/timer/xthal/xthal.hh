#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/xtensa/timer/xthal/interface.hh"

namespace bzd::components::esp32::timer {

class Xthal : public bzd::Timer
{
public:
	template <class Context>
	constexpr explicit Xthal(Context&) noexcept
	{
	}

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept final;

	bzd::Async<> exec() noexcept;

private:
	bzd::UInt64 ticks_{0};
};

} // namespace bzd::components::esp32::timer
