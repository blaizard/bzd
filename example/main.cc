#include "bzd.h"

#include <chrono>
#include <iostream>

void fct1();
void fct2();

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

class Awaiter
{
public:
	/**
	 * Used when polling
	 */
	bool isReady();
};

// Polling typ promise
// event type promise

template <class V, class E>
using PromiseReturnType = bzd::Optional<bzd::Result<V, E>>;

template <class V, class E, class T>
class Promise : public bzd::Promise<V, E>
{
private:
	using ReturnType = PromiseReturnType<V, E>;
	using bzd::Promise<V, E>::setResult;

public:
	using bzd::Promise<V, E>::isReady;

	Promise(T&& callack) : bzd::Promise<V, E>{}, callack_{bzd::forward<T>(callack)} {}
	bool poll() override { setResult(callack_()); return isReady(); }

private:
	const bzd::Function<ReturnType(void)> callack_;
};

template <class V, class E, class T>
Promise<V, E, T> makePromise(T&& callback)
{
	return Promise<V, E, T>(bzd::forward<T>(callback));
}

uint64_t getTimestampMs()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

auto delay(const int timeMs)
{
	const auto timestampMs = getTimestampMs();
	// 3 states: no result, resolve, reject
	return makePromise<uint64_t, int>([timestampMs, timeMs]() -> PromiseReturnType<uint64_t, int> {
		const auto currentTimestampMs = getTimestampMs();
		if (currentTimestampMs < timestampMs + timeMs)
		{
			return bzd::nullopt;
		}
		return currentTimestampMs;
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
	bzd::platform::Stack<2000> stack1;
	bzd::platform::Stack<2000> stack2;

	stack1.taint(0xaa);
	stack2.taint(0xaa);

	int i = 10;
	bzd::Task task1{[] {
		int i = 10;
		while (i > 0)
		{
			//auto promise = delay(500);
			auto result = await delay(500);
			//auto result = bzd::await(promise);
			std::cout << "Fct 1: " << i-- << "  " << *result << std::endl;
			bzd::yield();
		}
		std::cout << "Fct 1: end" << std::endl;
	}};

	bzd::Task task2{[&i] {
		while (i > 0)
		{
			std::cout << "Fct 2: " << i-- << std::endl;
			bzd::yield();
		}
		std::cout << "Fct 2: end" << std::endl;
	}};

	task1.bind(stack1);
	task2.bind(stack2);

	const auto promise = delay(12);
	std::cout << promise.isReady() << std::endl;
	std::cout << getTimestampMs() << std::endl;

	bzd::getScheduler().addTask(&task1);
	bzd::getScheduler().addTask(&task2);

	bzd::getScheduler().start();

	std::cout << "done!" << std::endl;
	std::cout << getTimestampMs() << std::endl;

	return 0;
}
