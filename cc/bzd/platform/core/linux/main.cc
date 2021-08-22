#include "cc/bzd/core/delay.h"
#include "cc/bzd/core/executor.h"
#include "cc/bzd/platform/core/linux/core.h"
#include "cc/bzd/platform/core/linux/executor.h"
#include "cc/bzd/platform/core/linux/pi.h"

#include <iostream>

auto& Registry()
{
	// Definition for all registry entries.
	static bzd::platform::linux::Core<100000> bzd_linux0{0};
	static bzd::platform::linux::Core<100000> bzd_linux1{1};
	static bzd::platform::core::Executor executor{bzd_linux0, bzd_linux1};

	// Registry structure containing a reference of all entries.
	struct Registry
	{
		decltype(bzd_linux0)& bzd_linux0;
		decltype(bzd_linux1)& bzd_linux1;
		decltype(executor)& executor;
	};
	static Registry registry{bzd_linux0, bzd_linux1, executor};

	return registry;
}

// registry;

/*
// linux 4 cores composition file.
// This defines available executors for the system.
executors linux
{
	// Can share payload
	Linux core0(100000, 0);
	Linux core1(100000, 1);
	Linux core2(100000, 2);
	Linux core3(100000, 3);

	Executor linux(core0, core1, core2, core3);
}

executors AurixCore0
{
	Aurix core(1000000, 0);
}

executors safety [critical]
{
	Linux core(100000, 0);
}

executors esp32
{
	FreeRTOS core0(4000, 0);
	FreeRTOS core1(10000, 1);
}

composition
{
	channel<Shmem> com(linux, safety);
	channel<Ethernet> temp(linux, esp32);
}

// MCU specific, instanciate the i2c driver.
composition
{
	I2C i2c;

	periodic(caller=i2c.run, time=50ms);
}

// Board level, assign i2c instance to sensor
composition
{
	Tempsensor sensor(i2c);

	exec(caller=sensor.run);
}

// Functional level specific.
// This can only run on a critical core and the exec and periodic calls
// will always run in the same core, hence sequencially.
composition [critical, same]
{
	Monitor monitor;

	exec(caller=monitor.run);

	periodic(caller=monitor.print, time=10ms);

	connect(input=sensor.data, output=monitor.input);
}

// Find an executor given specific constraints.
getExecutor(Contraints ...)
*/

bzd::Async<void> task1() noexcept
{
	for (int i = 0; i < 4; ++i)
	{
		auto t1 = delay(500_ms);
		auto t2 = delay(1_s);
		co_await bzd::async::all(t1, t2);
		std::cout << "." << std::endl;
	}
}

/*
void payload(bzd::core::Executor& executor)
{
	auto promise = task1();

	executor.run(promise);
}
*/

int main()
{
	auto& registry = Registry();

	// auto promise = task1();

	auto promise = calculatePi<2>();
	registry.executor.enqueue(promise);

	registry.executor.start();

	registry.executor.stop();

	/*
		auto workload = [&executor]() { executor.run(); };
		registry.linux0.start(static_cast<Callable>(workload));
		registry.linux1.start(static_cast<Callable>(workload));

		registry.linux1.stop();
		registry.linux0.stop();
	*/
	std::cout << registry.bzd_linux0.getId() << ": " << registry.bzd_linux0.base().getStackUsage() << std::endl;
	std::cout << registry.bzd_linux1.getId() << ": " << registry.bzd_linux1.base().getStackUsage() << std::endl;

	// bzd::core::Executor

	// Start all executors
	// ....
	// Assign workload to  executors based on their constraints
	// ....

	/*bzd::core::Executor executor{registry.linux1};

	executor.start();

	payload(executor);

	// task1.sync();
	// executor.dispatch(task1, 23);

	auto workload = []() { std::cout << "Workload" << std::endl; };
	registry.linux1.start(static_cast<Callable>(workload));

	std::cout << registry.linux1.getStackUsage() << std::endl;

	executor.stop();*/

	return 0;
}
