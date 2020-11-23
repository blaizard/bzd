#pragma once

#include "bzd/core/promise.h"
#include "bzd/core/units.h"
#include "bzd/platform/clock.h"

namespace bzd {
inline auto delay(const bzd::units::Millisecond time)
{
	auto duration = bzd::platform::getTicks().toDuration();
	const auto targetDuration = duration + bzd::platform::msToTicks(time);
	return bzd::makePromise([duration, targetDuration]() mutable -> bzd::PromiseReturnType<> {
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
