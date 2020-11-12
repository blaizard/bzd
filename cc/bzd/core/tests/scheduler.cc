#include "bzd/core/scheduler.h"

#include "bzd/core/task.h"
#include "bzd/platform/stack.h"
#include "cc_test/test.h"

TEST(Scheduler, SimpleScheduling)
{
	bzd::String<100> output;

	static bzd::platform::Stack<1000> stackA;
	static bzd::platform::Stack<1000> stackB;

	bzd::Task taskA{[&output] {
		for (int i = 0; i < 10; ++i)
		{
			output += 'A';
			std::cout << "A" << std::endl;
			bzd::yield();
		}
		std::cout << "done A" << std::endl;
	}};
	taskA.bind(stackA);
	bzd::getScheduler().addTask(&taskA);

	bzd::Task taskB{[&output] {
		for (int i = 0; i < 10; ++i)
		{
			output += 'B';
			std::cout << "B" << std::endl;
			bzd::yield();
		}
		std::cout << "done B" << std::endl;
	}};
	taskB.bind(stackB);
	bzd::getScheduler().addTask(&taskB);

	bzd::getScheduler().start();
	std::cout << "OUT!" << std::endl;
}
