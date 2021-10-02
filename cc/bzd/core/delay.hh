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
	auto duration = clock.getTicks().toDuration();
	const auto targetDuration = duration + clock.msToTicks(time);

	do
	{
		co_await bzd::async::yield();

		const auto curTicks = clock.getTicks();

		// Update the current duration and update the wrapping counter
		auto details = duration.getDetails();
		if (details.ticks > curTicks.get())
		{
			++details.wrappingCounter;
		}
		details.ticks = curTicks.get();
		duration.setFromDetails(details);

		// Check if the duration is reached
	} while (duration < targetDuration);
}

} // namespace bzd
