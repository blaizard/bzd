#include "cc/bzd/core/async.h"
#include "cc/bzd/core/delay.h"
#include "cc/bzd/platform/clock.h"

#include <iostream>

bzd::Async<void> task1() noexcept
{
	for (int i = 0; i < 10; ++i)
	{
		auto task1 = delay(500_ms);
		auto task2 = delay(1_s);
		co_await bzd::async::all(task1, task2);
		std::cout << "." << std::endl;
	}
}

int main()
{
	std::cout << "start" << std::endl;
	auto promise = task1();
	bzd::async::run(promise);
	std::cout << "end" << std::endl;
	return 0;
}
