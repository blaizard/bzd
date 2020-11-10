#include "bzd/core/scheduler.h"

#include "bzd/core/task.h"
#include "bzd/platform/stack.h"
#include "cc_test/test.h"

TEST(Scheduler, SimpleScheduling)
{
	bzd::String<100> output;

	bzd::platform::Stack<100> stackA;
	bzd::Task taskA{[&output] {
		for (int i = 0; i < 10; ++i)
		{
			output += 'A';
			bzd::yield();
		}
	}};
	taskA.bind(stackA);
	bzd::getScheduler().addTask(taskA);

	bzd::platform::Stack<100> stackB;
	bzd::Task taskB{[&output] {
		for (int i = 0; i < 10; ++i)
		{
			output += 'B';
			bzd::yield();
		}
	}};
	taskB.bind(stackB);
	bzd::getScheduler().addTask(taskB);

	// bzd::getScheduler().start();
}
