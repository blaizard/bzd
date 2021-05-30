#include "cc/bzd/platform/core/linux/core.h"

#include <iostream>

int main()
{
	static bzd::platform::core::Linux<100000> linux{};
	auto workload = []() { std::cout << "Workload" << std::endl; };
	linux.run(static_cast<Callable>(workload));

	std::cout << linux.getStackUsage() << std::endl;

	return 0;
}
