#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/clock.hh"

namespace bzd::platform::esp32::clock {

class Xthal : public bzd::Clock2
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

} // namespace bzd::platform::esp32::clock
