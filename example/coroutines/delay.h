#pragma once

#include "bzd/core/units.h"
#include "bzd/platform/clock.h"

namespace bzd {
/**
 * Pauses the program for the amount of time (in milliseconds) specified as parameter.
 *
 * \param time Time to pause the program for.
 *
 * \return A promise object.
 */
inline auto delay(const bzd::units::Millisecond time) noexcept
{
	auto duration = bzd::platform::getTicks().toDuration();
	const auto targetDuration = duration + bzd::platform::msToTicks(time);
	return bzd::makePromise([duration, targetDuration](bzd::interface::Promise&, bzd::AnyReference&) mutable -> bzd::Promise<>::ReturnType {
		const auto curTicks = bzd::platform::getTicks();

		// Update the current duration and update the wrapping counter
		auto details = duration.getDetails();
		if (details.ticks > curTicks.get())
		{
			++details.wrappingCounter;
		}
		details.ticks = curTicks.get();
		duration.setFromDetails(details);

		// Check if the duration is reached
		if (duration >= targetDuration)
		{
			return bzd::nullresult;
		}
		return bzd::nullopt;
	});
}
} // namespace bzd
