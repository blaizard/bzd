#include "cc/components/std/clock/system/system.hh"

#include <chrono>

namespace bzd::platform::std::clock {

bzd::Result<bzd::units::Millisecond, bzd::Error> System::getTime() noexcept
{
	return ::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace bzd::platform::std::clock
