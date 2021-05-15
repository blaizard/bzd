#include "bzd/core/units.h"
#include "bzd/platform/clock.h"
#include "example/coroutines/async.h"

#include <iostream>

bzd::Async<void> delay(const bzd::units::Millisecond time) noexcept
{
	auto duration = bzd::platform::getTicks().toDuration();
	const auto targetDuration = duration + bzd::platform::msToTicks(time);

	do
	{
		co_await bzd::async::yield();

		const auto curTicks = bzd::platform::getTicks();

		// Update the current duration and update the wrapping counter
		auto details = duration.getDetails();
		if (details.ticks > curTicks.get())
		{
			++details.wrappingCounter;
		}
		details.ticks = curTicks.get();
		duration.setFromDetails(details);

		// std::cout << duration.get() << " - " << targetDuration.get() << std::endl;

		// Check if the duration is reached
	} while (duration < targetDuration);
}

bzd::Async<void> task1() noexcept
{
	for (int i = 0; i < 10; ++i)
	{
		co_await delay(1_s);
		std::cout << "." << std::endl;
	}
}

int main()
{
	std::cout << "start" << std::endl;
	auto promise = task1();
	promise.sync();
	std::cout << "end" << std::endl;
	return 0;
}
