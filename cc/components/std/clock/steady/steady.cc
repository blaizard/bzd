#include "cc/components/std/clock/steady/steady.hh"

#include <chrono>

namespace bzd::platform::std::clock {

bzd::Result<bzd::units::Millisecond, bzd::Error> Steady::getTime() noexcept
{
	return ::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch()).count();
}

} // namespace bzd::platform::std::clock
