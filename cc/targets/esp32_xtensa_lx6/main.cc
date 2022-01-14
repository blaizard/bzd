#include "cc/bzd.hh"
#include "cc/components/std/stream/out/out.hh"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <iostream>

bzd::Async<void> nopVoid2(int a, int b, int c)
{
	std::cout << a << " " << b << " " << c << std::endl;
	co_return;
}

bzd::Async<void> nopVoid(int a, int b, int c)
{
	for (auto i = 0; i<a+b+c; ++i)
	{
	std::cout << a << " " << b << " " << c << std::endl;
	co_await nopVoid2(a, b, c);
	}
	co_return;
}

constexpr void appendToTrace(bzd::interface::String& trace, bzd::StringView id, int checkpoint)
{
	trace += '[';
	trace += id;
	toString(trace, checkpoint);
	trace += ']';
}

bzd::Async<int> nop(bzd::interface::String& trace, bzd::StringView id, int retVal)
{
	appendToTrace(trace, id, 0);
	co_return retVal;
}

bzd::Async<int> nested(bzd::interface::String& trace, bzd::StringView id, int retVal = 42)
{
	appendToTrace(trace, id, 1);
	auto result = co_await nop(trace, id, retVal);
	appendToTrace(trace, id, 2);
	co_return result;
}

bzd::Async<int> deepNested(bzd::interface::String& trace, bzd::StringView id)
{
	int result = 0;
	for (int i = 0; i < 3; ++i)
	{
		result += i;
		appendToTrace(trace, id, 3);
		co_await nested(trace, id);
		appendToTrace(trace, id, 4);
	}
	co_return bzd::move(result);
}

bzd::Async<int> asyncAllNested(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 5);
	auto promiseY = nested(trace, "y");
	auto promiseZ = nested(trace, "z");
	co_await bzd::async::all(promiseY, promiseZ);
	appendToTrace(trace, id, 6);

	co_return 12;
}

bzd::Async<int> asyncAdd(int a, int b)
{
	co_return a + b;
}

bzd::Async<int> asyncFibonacci(int n)
{
	if (n <= 2) co_return 1;

	int a = 1;
	int b = 1;

	// iterate computing fib(n)
	for (int i = 0; i < n - 2; ++i)
	{
		const auto c = co_await asyncAdd(a, b);
		a = b;
		b = c.value();
	}

	co_return b;
}

void mainTask(void*)
{
	std::cout << "START" << std::endl;

	{
		bzd::String<128> trace;
		auto promiseA = asyncAllNested(trace, "a");
		auto promiseB = deepNested(trace, "b");
		bzd::async::all(promiseA, promiseB).sync();
		std::cout << trace.data() << std::endl;
	}

	{
		bzd::String<128> trace;
		auto promiseA = nested(trace, "a");
		auto promiseB = deepNested(trace, "b");
		bzd::async::any(promiseA, promiseB).sync();
		std::cout << trace.data() << std::endl;
	}

	{
		bzd::String<128> trace;
		auto promiseA = deepNested(trace, "a");
		auto promiseB = deepNested(trace, "b");
		auto promiseC = nested(trace, "c", -432);
		auto promiseD = deepNested(trace, "d");
		bzd::async::any(bzd::move(promiseA), bzd::move(promiseB), bzd::move(promiseC), bzd::move(promiseD)).sync();
		std::cout << trace.data() << std::endl;
	}

	{
		auto fibonacci1 = asyncFibonacci(12);
		auto fibonacci2 = asyncFibonacci(16);
		auto fibonacci3 = asyncFibonacci(18);
		auto fibonacci4 = asyncFibonacci(20);
		const auto result = bzd::async::all(fibonacci1, fibonacci2, fibonacci3, fibonacci4).sync();
		std::cout << result.get<3>().value() << std::endl;
	}

	bzd::platform::std::Out out{};

	auto c = toStream(out, CSTR("BZD {} numbers {} {}"), "dsds"_sv, 5, 6);

	bzd::async::all(c).sync();
	nopVoid(1, 2, 3).sync();
	nopVoid(1, 2, 3).sync();
	nopVoid(1, 2, 3).sync();
	nopVoid(1, 2, 3).sync();

	std::cout << "END" << std::endl;
	while (true)
	{
	};
}

int main()
{
//	constexpr auto stackSize = 10000;
//	constexpr auto priority = 5;

	mainTask(nullptr);
//	xTaskCreate(mainTask, "main task", stackSize, nullptr, priority, nullptr);

	// Prevent the program to exit.
	while (true)
	{
	};

	return 0;
}
