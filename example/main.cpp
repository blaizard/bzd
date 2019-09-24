#include <iostream>

#include "include/scheduler.h"
#include "include/stack.h"
#include "include/task.h"

void fct1();
void fct2();

bzd::Stack<2000> stack1;
bzd::Stack<2000> stack2;
bzd::Task<decltype(fct1)> task1(fct1);
bzd::Task<decltype(fct2)> task2(fct2);

int i = 0;

void fct1()
{
	while (i < 10)
	{
		std::cout << "Fct 1: " << i++ << std::endl;
		bzd::yield();
	}
}


void fct2()
{
	while (i < 10)
	{
		std::cout << "Fct 2: " << i++ << std::endl;
		bzd::yield();
	}
}

int main()
{
	task1.bind(stack1);
	task2.bind(stack2);

	bzd::Scheduler::getInstance().addTask(task1);
	bzd::Scheduler::getInstance().addTask(task2);

	bzd::Scheduler::getInstance().start();

	return 0;
}
