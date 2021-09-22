#include "bzd.hh"
#include "example/mutex.hh"
#include "example/terminal.hh"

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

void simpleDelay()
{
	int i = 20;
	static auto task1 = bzd::makeTask<10000>([&i] {
		while (i > 0)
		{
			await bzd::delay(1_s);
			std::cout << "Fct 1: " << i-- << " (1s)" << std::endl;
		}
		std::cout << "Fct 1: end" << std::endl;
	});

	static auto task2 = bzd::makeTask<10000>([&i] {
		while (i > 0)
		{
			std::cout << "Fct 2: " << i-- << " (200ms)" << std::endl;
			await bzd::delay(200_ms);
		}
		std::cout << "Fct 2: end" << std::endl;
	});

	bzd::getScheduler().addTask(task1);
	bzd::getScheduler().addTask(task2);

	bzd::getScheduler().start();

	std::cout << "Max stack usage stack1: " << task1.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack2: " << task2.estimateMaxUsage() << std::endl;
}

void exampleMutex()
{
	Mutex mutex{};

	static auto task1 = bzd::makeTask<10000>([&mutex] {
		for (int i = 0; i < 3; ++i)
		{
			await mutex.lock();
			std::cout << "<1.." << std::flush;
			await bzd::delay(100_ms);
			std::cout << "..1>" << std::endl;
			mutex.unlock();
		}
	});

	static auto task2 = bzd::makeTask<10000>([&mutex] {
		for (int i = 0; i < 3; ++i)
		{
			await mutex.lock();
			std::cout << "<2.." << std::flush;
			await bzd::delay(100_ms);
			std::cout << "..2>" << std::endl;
			mutex.unlock();
		}
	});

	static auto task3 = bzd::makeTask<10000>([&mutex] {
		for (int i = 0; i < 3; ++i)
		{
			await mutex.lock();
			std::cout << "<3.." << std::flush;
			await bzd::delay(100_ms);
			std::cout << "..3>" << std::endl;
			mutex.unlock();
		}
	});

	bzd::getScheduler().addTask(task1);
	bzd::getScheduler().addTask(task2);
	bzd::getScheduler().addTask(task3);

	bzd::getScheduler().start();

	std::cout << "Max stack usage stack1: " << task1.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack2: " << task2.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack3: " << task3.estimateMaxUsage() << std::endl;
}

void terminalTask()
{
	Terminal terminal{};

	{
		const bzd::String<2> prompt{"> "};
		const auto bytes = prompt.asBytes();
		await terminal.write(bytes);
	}
	/*
		bzd::String<64> input;
		bzd::Array<char, 10> data;
		auto bytes = data.asWritableBytes();
		const auto result = await terminal.read(bytes);
	*/
}

void exampleTerminal()
{
	Terminal terminal{};

	bool isExit{false};
	static auto task1 = bzd::makeTask<10000>([&isExit] {
		int i = 0;
		while (isExit == false && i < 20)
		{
			await bzd::delay(200_ms);
			std::cout << "." << std::flush;
			++i;
		}
		isExit = true;
	});

	static auto task2 = bzd::makeTask<10000>([&isExit, &terminal] {
		terminalTask();

		std::cout << "Press 'q' to exit." << std::endl;
		while (isExit == false)
		{
			bzd::Array<char, 10> data;
			auto bytes = data.asWritableBytes();

			const auto result = await terminal.read(bytes);
			bzd::assert::isTrue(result.hasValue(), "I/O error");

			const auto out = bytes.subSpan(0, result.value());
			await terminal.write(out);

			if (data[0] == 'q')
			{
				isExit = true;
			}
		}
	});

	bzd::getScheduler().addTask(task1);
	bzd::getScheduler().addTask(task2);

	bzd::getScheduler().start();

	std::cout << std::endl;
	std::cout << "Max stack usage stack1: " << task1.estimateMaxUsage() << std::endl;
	std::cout << "Max stack usage stack2: " << task2.estimateMaxUsage() << std::endl;
}

int main()
{
	//	simpleDelay();
	//	exampleMutex();
	exampleTerminal();
}
