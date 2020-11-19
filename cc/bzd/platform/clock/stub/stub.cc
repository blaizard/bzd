#include "bzd/platform/clock.h"

bzd::ClockTickType bzd::platform::getTicks() noexcept
{
	static bzd::ClockTickType counter = 0;
	return ++counter;
}
