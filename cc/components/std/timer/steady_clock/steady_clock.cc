#include "cc/components/std/timer/steady_clock/steady_clock.hh"

#include <chrono>

namespace bzd::components::std::timer {

bzd::Result<bzd::units::Millisecond, bzd::Error> SteadyClock::getTime() noexcept
{
	return ::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch()).count();
}

} // namespace bzd::components::std::timer
