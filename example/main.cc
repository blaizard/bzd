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
		return bzd::makePromise(
			[duration, targetDuration](bzd::interface::Promise&, bzd::AnyReference&) mutable -> bzd::Promise<>::ReturnType {
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
		std::cout << "Press 'q' to exit." << std::endl;
		while (isExit == false)
		{
			bzd::String<1> data{" "};
			await bzd::delay(500_ms);
			auto promiseTerminal = terminal.read(data.asWritableBytes());
			//auto promiseTerminal = bzd::delay(500_ms);
			//bzd::ignore = bzd::Promise<bzd::SizeType>();

			//bzd::PromiseOr promiseOr{promiseTerminal, promiseDelay};
			await promiseTerminal;

			if (data[0] == 'q')
			{
				isExit = true;
			}
			std::cout << "[" << data[0] << "]" << std::endl;
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
	//simpleDelay();
	//exampleMutex();
	exampleTerminal();
}
