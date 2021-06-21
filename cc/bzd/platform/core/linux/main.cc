#include "cc/bzd/platform/core/linux/core.h"
#include "cc/bzd/core/executor.h"
#include "cc/bzd/core/async.h"
#include "cc/bzd/core/delay.h"

#include <iostream>

// registry.h
struct
{
	bzd::platform::core::Linux<100000> linux{1};
} registry;

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
// This can only run on a critical core
composition [critical]
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

void payload()
{
	auto promise = task1();
	auto promiseAll = bzd::async::all(promise);

	bzd::async::run(promiseAll);
}

int main()
{
	// Start all executors
	// ....
	// Assign workload to  executors based on their constraints
	// ....

	bzd::core::Executor executor{registry.linux};

	executor.start();

	payload();

	//task1.sync();
	// executor.dispatch(task1, 23);

	auto workload = []() { std::cout << "Workload" << std::endl; };
	registry.linux.run(static_cast<Callable>(workload));

	std::cout << registry.linux.getStackUsage() << std::endl;

	executor.stop();

	return 0;
}
