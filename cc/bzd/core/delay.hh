#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/clock.hh"

namespace bzd {
/**
 * Pauses the program for the amount of time (in milliseconds) specified as parameter.
 *
 * \param time Time to pause the program for.
 *
 * \return A promise object.
 */
bzd::Async<void> delay(const bzd::units::Millisecond time) noexcept
{
	auto& clock = bzd::platform::steadyClock();
	const auto targetTicks = clock.getTicks() + clock.msToTicks(time);

	do
	{
		co_await bzd::async::yield();
		// Check if the duration is reached
	} while (clock.getTicks() < targetTicks);
}

} // namespace bzd
