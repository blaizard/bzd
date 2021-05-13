#include "bzd/core/units.h"
#include "bzd/platform/clock.h"
#include "example/coroutines/async.h"

#include <iostream>

bzd::Async<void> delay(const bzd::units::Millisecond time) noexcept
{
	auto duration = bzd::platform::getTicks().toDuration();
	const auto targetDuration = duration + bzd::platform::msToTicks(time);

	std::cout << "Start" << std::endl;

	do
	{
		co_await bzd::impl::SuspendAlways{};
		/*
				{
					auto&& awaiter = bzd::impl::SuspendAlways{};
					if (!awaiter.await_ready()) {
						awaiter.await_suspend(std::coroutine_handle<> p);
						// compiler added suspend/resume hook
					}
					awaiter.await_resume();
				}
		*/

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

	co_return;
}

bzd::Async<void> task1() noexcept
{
	for (int i = 0; i < 10; ++i)
	{
		co_await delay(1_s);
		std::cout << "." << std::endl;
	}

	co_return;
}

int main()
{
	std::cout << "start" << std::endl;
	auto promise = task1();
	promise.sync();
	std::cout << "end" << std::endl;
	return 0;
}
