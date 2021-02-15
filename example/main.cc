#include "bzd.h"
#include "example/mutex.h"
#include "example/terminal.h"

#include <iostream>
#include <thread>

/**
Uses cases for promises:

**** Event triggered ****

----- Result triggered by global function

nonOwnlingList list{};

void __interrupt_handler()
{
	for (promise in list)
	{
		promise.setResult();
	}
	clear_interrupt();
}

void __interrupt_handler_error()
{
	for (promise in list)
	{
		promise.setError();
	}
}

----- Result triggered by class member function

class Driver
{
private:
	void callback()
	{
		for (promise in list_)
		{
			promise.setResult();
		}
	}
	nonOwnlingList<Promise> list_{};
};

----- Deregister promise from user application

{
	auto promise = await DoThings();
	...
}
<- out of scope, destructor should deregister the promise:
	~Promise {
		deregister();
	}

**** Polling ****


*/

/**
 * Polling implementation.
 */
class Delay
{
public:
	void waitForEventNoArgs() { waitForEvent(1_s); }

	void waitForEvent(const bzd::units::Millisecond time)
	{
		std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(time.get()));
		std::cout << "Event!" << std::endl;
	}

	auto delayEvent(const bzd::units::Millisecond time)
	{
		static std::thread first([this, time] { this->waitForEvent(time); });
		return bzd::nullopt;
	}

	auto delayPolling(const bzd::units::Millisecond time)
	{
		auto duration = bzd::platform::getTicks().toDuration();
		const auto targetDuration = duration + bzd::platform::msToTicks(time);
		return bzd::makePromise([duration, targetDuration](bzd::interface::Promise&) mutable -> bzd::Promise<>::ReturnType {
			const auto curTicks = bzd::platform::getTicks();

			// Update the current duration and update the wrapping counter
			auto details = duration.getDetails();
			if (details.ticks > curTicks.get())
			{
				++details.wrappingCounter;
			}
			details.ticks = curTicks.get();
			duration.setFromDetails(details);

			// Check if the duration is reached
			if (duration >= targetDuration)
			{
				return bzd::nullresult;
			}
			return bzd::nullopt;
		});
	}
};

void simpleDelay()
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
}

void exampleMutex()
{
	static bzd::platform::Stack<10000> stack1;
	static bzd::platform::Stack<10000> stack2;
	static bzd::platform::Stack<10000> stack3;

	stack1.taint();
	stack2.taint();
	stack3.taint();

	Mutex mutex{};

	bzd::Task task1{[&mutex] {
		for (int i = 0; i < 3; ++i)
		{
			await mutex.lock();
			std::cout << "<1.." << std::flush;
			await bzd::delay(100_ms);
			std::cout << "..1>" << std::endl;
			mutex.unlock();
		}
	}};

	bzd::Task task2{[&mutex] {
		for (int i = 0; i < 3; ++i)
		{
			await mutex.lock();
			std::cout << "<2.." << std::flush;
			await bzd::delay(100_ms);
			std::cout << "..2>" << std::endl;
			mutex.unlock();
		}
	}};

	bzd::Task task3{[&mutex] {
		for (int i = 0; i < 3; ++i)
		{
			await mutex.lock();
			std::cout << "<3.." << std::flush;
			await bzd::delay(100_ms);
			std::cout << "..3>" << std::endl;
			mutex.unlock();
		}
	}};

	task1.bind(stack1);
	task2.bind(stack2);
	task3.bind(stack3);

	bzd::getScheduler().addTask(task1);
	bzd::getScheduler().addTask(task2);
	bzd::getScheduler().addTask(task3);

	bzd::getScheduler().start();

	std::cout << "Max stack usage stack1: " << stack1.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack2: " << stack2.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack3: " << stack3.estimateMaxUsage() << std::endl;
}

void exampleTerminal()
{
	Terminal terminal;
	static bzd::platform::Stack<10000> stack1;
	static bzd::platform::Stack<10000> stack2;

	stack1.taint();
	stack2.taint();

	bool isExit{false};
	bzd::Task task1{[&isExit] {
		while (isExit == false)
		{
			await bzd::delay(1_s);
			std::cout << "." << std::flush;
		}
	}};

	bzd::Task task2{[&isExit, &terminal] {
		std::cout << "Press 'q' to exit." << std::endl;
		while (isExit == false)
		{
			bzd::String<1> data{" "};
			await terminal.read(data.asWritableBytes());
			if (data[0] == 'q')
			{
				isExit = true;
			}
			std::cout << data[0] << std::flush;
		}
	}};

	task1.bind(stack1);
	task2.bind(stack2);

	bzd::getScheduler().addTask(task1);
	bzd::getScheduler().addTask(task2);

	bzd::getScheduler().start();

	std::cout << std::endl;
	std::cout << "Max stack usage stack1: " << stack1.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack2: " << stack2.estimateMaxUsage() << std::endl;
}

int main()
{
	// simpleDelay();
	// exampleMutex();
	exampleTerminal();
}
