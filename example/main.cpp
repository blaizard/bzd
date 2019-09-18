#include <iostream>

#include "include/stack.h"
#include "include/task.h"
#include "include/interface/task.h"

void fct1();
void fct2();

//async::Stack<2000> stack1;
//async::Task<decltype(fct1)> task1(stack1, fct1);
async::Stack<2000> stack2;
//async::Task<void()> task2(stack2, fct2);

#include <functional>

class test
{
public:
	test(int i) : i_(i) {}

	void print()
	{
		std::cout << "Value passed is " << i_ << std::endl;
		std::exit(0);
	}

private:
	int i_;
};

test temp1(42);
/*
std::function<void()> fct = []() {
	std::cout << "Fct 2 start" << std::endl;
};

async::Task<decltype(fct)> task2(stack2, fct);
*/

async::Task<test*, decltype(&test::print)> task3(stack2, &temp1, &test::print);

//async::interface::Task task2(stack2, &temp1, (async::fct_ptr_type)&test::print);

/*
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
*/
int main()
{
	task3.start();

	//task2.start();
	//task1.start();

	return 0;
}
