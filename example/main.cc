#include "bzd.h"

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
	void waitForEventNoArgs()
	{
		waitForEvent(1_s);
	}

	void waitForEvent(const bzd::units::Millisecond time)
	{
		std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(time.get()));
		std::cout << "Event!" << std::endl;
	}

	auto delayEvent(const bzd::units::Millisecond time)
	{
		static std::thread first([this, time] {
			this->waitForEvent(time);
		});
		return bzd::nullopt;
	}

	auto delayPolling(const bzd::units::Millisecond time)
	{
		auto duration = bzd::platform::getTicks().toDuration();
		const auto targetDuration = duration + bzd::platform::msToTicks(time);
		return bzd::makePromise([duration, targetDuration]() mutable -> bzd::PromiseReturnType<> {
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

class FunctionPointer
{
public:
	template <class Object, class T>
	constexpr FunctionPointer(Object& obj, T memberPtr) : obj_{&obj}, callable_{[memberPtr](void* ptr) {
		(reinterpret_cast<Object*>(ptr)->*memberPtr)();
	}}
	{
	}

/*	template <class T>
	constexpr FunctionPointer(T* fctPtr) : obj_{nullptr}, callable_{[](void*) {
		(reinterpret_cast<Object*>(ptr)->*memberPtr)();
	}}
	{
	}
*/
	void call()
	{
		return callable_(obj_);
	}

private:
	void* obj_;
	bzd::Function<void(void*)> callable_;
};

int main()
{
	static bzd::platform::Stack<10000> stack1;
	static bzd::platform::Stack<10000> stack2;

	Delay object{};

	stack1.taint();
	stack2.taint();

	FunctionPointer abs{object, &decltype(object)::waitForEventNoArgs};
	abs.call();

	//object.waitForEvent(1_s);

	int i = 20;
	bzd::Task task1{[&i, &object] {

		std::cout << "> Delay polling 1s..." << std::endl;
		await object.delayPolling(1_s);
		std::cout << "< Delay polling 1s" << std::endl;

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
