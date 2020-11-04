#include "bzd.h"
#include "example/scheduler.h"
#include "example/stack.h"
#include "example/task.h"

#include <chrono>
#include <iostream>

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

/*
template <class V, class E>
class Promise : public bzd::Result<V, E>
{
public:
	virtual Result<bool> isReady() = 0;
};
*/

template <class T>
class Promise
{
public:
	Promise(T&& callack) : callack_{callack} {}
	bool isReady() const { return callack_(); }

private:
	T callack_;
};

uint64_t getTimestampMs()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

auto delay(const int timeMs)
{
	const auto timestampMs = getTimestampMs();
	return Promise([timestampMs, timeMs]() {
		const auto currentTimestampMs = getTimestampMs();
		if (currentTimestampMs < timestampMs + timeMs)
		{
			return false;
		}
		return true;
	});
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

	const auto promise = delay(12);
	std::cout << promise.isReady() << std::endl;
	std::cout << getTimestampMs() << std::endl;

	bzd::Scheduler::getInstance().addTask(task1);
	bzd::Scheduler::getInstance().addTask(task2);

	bzd::Scheduler::getInstance().start();

	return 0;
}
