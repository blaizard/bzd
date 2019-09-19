#include <iostream>

#include "include/stack.h"
#include "include/task.h"

void fct1();
void fct2();

async::Stack<2000> stack1;
async::Stack<2000> stack2;
async::Task<decltype(fct1)> task1(fct1);
async::Task<decltype(fct2)> task2(fct2);

void fct1()
{
	std::cout << "Fct 1 start" << std::endl;
	task1.yield(task2);
	std::cout << "Fct 1 end" << std::endl;
}

void fct2()
{
	std::cout << "Fct 2 start" << std::endl;
	task2.yield(task1);
	std::cout << "Fct 2 end" << std::endl;
}

int main()
{
	task1.bind(stack1);
	task2.bind(stack2);

	task1.start();
/*
	async::Scheduler scheduler(stack1, stack2);

	scheduler.addTask(task1, task2);

	scheduler.start();
*/
	return 0;
}
