#include "bzd.h"

#include <iostream>

int main()
{
	static bzd::platform::Stack<10000> stack1;
	static bzd::platform::Stack<10000> stack2;

	stack1.taint();
	stack2.taint();

	int i = 20;
	bzd::Task task1{[&i] {
		while (i > 0)
		{
			await bzd::delay(1_s);
			std::cout << "Fct 1: " << i-- << " (1s)" << std::endl;
		}
		std::cout << "Fct 1: end" << std::endl;
	}};

	bzd::Task task2{[&i] {
		while (i > 0)
		{
			std::cout << "Fct 2: " << i-- << " (200ms)" << std::endl;
			await bzd::delay(200_ms);
		}
		std::cout << "Fct 2: end" << std::endl;
	}};

	task1.bind(stack1);
	task2.bind(stack2);

	bzd::getScheduler().addTask(task1);
	bzd::getScheduler().addTask(task2);

	bzd::getScheduler().start();

	std::cout << "Max stack usage stack1: " << stack1.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack2: " << stack2.estimateMaxUsage() << std::endl;

	return 0;
}
