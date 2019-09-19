#include <iostream>

#include "include/scheduler.h"
#include "include/stack.h"
#include "include/task.h"

void fct1();
void fct2();

async::Stack<2000> stack1;
async::Stack<2000> stack2;
async::Task<decltype(fct1)> task1(fct1);
async::Task<decltype(fct2)> task2(fct2);
async::Scheduler scheduler;

int i = 0;

void fct1()
{
	while (i < 10)
	{
		std::cout << "Fct 1: " << i++ << std::endl;
		scheduler.yield();
	}
}

void fct2()
{
	while (i < 10)
	{
		std::cout << "Fct 2: " << i++ << std::endl;
		scheduler.yield();
	}
}

int main()
{
	task1.bind(stack1);
	task2.bind(stack2);

	scheduler.addTask(task1);
	scheduler.addTask(task2);

	scheduler.start();
/*
	async::Scheduler<4, 1000> scheduler(stack1, stack2);

	scheduler.addTask(task1, task2);

	scheduler.start();
*/
	return 0;
}
