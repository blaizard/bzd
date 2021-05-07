#include "bzd/container/string.h"
#include "bzd/utility/format/integral.h"
#include "cc_test/test.h"
#include "example/coroutines/async.h"

#include <iostream>

namespace {
constexpr void appendToTrace(bzd::interface::String& trace, bzd::StringView id, int checkpoint)
{
	trace += '[';
	trace += id;
	bzd::format::appendToString(trace, checkpoint);
	trace += ']';
}
} // namespace

bzd::Async<int, int> nop(bzd::interface::String& trace, bzd::StringView id, int retVal)
{
	appendToTrace(trace, id, 0);
	co_return retVal;
}

TEST(Coroutine, Base)
{
	bzd::String<32> trace;
	auto promise = nop(trace, "a", 42);
	auto result = promise.sync();
	EXPECT_EQ(trace, "[a0]");
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 42);
}

bzd::Async<int, int> nested(bzd::interface::String& trace, bzd::StringView id, int retVal = 42)
{
	appendToTrace(trace, id, 1);
	auto result = co_await nop(trace, id, retVal);
	appendToTrace(trace, id, 2);
	co_return result;
}

TEST(Coroutine, Nested)
{
	bzd::String<32> trace;
	auto promise = nested(trace, "a");
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][a0][a2]");
}

bzd::Async<int, int> deepNested(bzd::interface::String& trace, bzd::StringView id)
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

TEST(Coroutine, DeepNested)
{
	bzd::String<128> trace;
	auto promise = deepNested(trace, "a");
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a3][a1][a0][a2][a4][a3][a1][a0][a2][a4][a3][a1][a0][a2][a4]");
}

TEST(Coroutine, waitAll)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a", 10);
	auto promiseB = nested(trace, "b", -4);
	auto promise = bzd::waitAll(promiseA, promiseB);
	auto result = promise.sync();
	EXPECT_EQ(trace, "[a1][b1][a0][a2][b0][b2]");
	EXPECT_EQ(result.size(), 2);
	EXPECT_TRUE(result.get<0>());
	EXPECT_EQ(result.get<0>().value(), 10);
	EXPECT_EQ(result.get<1>().value(), -4);
}

TEST(Coroutine, waitAllDifferent)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promise = bzd::waitAll(promiseA, promiseB);
	promise.sync();
	EXPECT_EQ(trace, "[a1][b3][a0][a2][b1][b0][b2][b4][b3][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
}

TEST(Coroutine, waitAllMany)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = nested(trace, "b");
	auto promiseC = nested(trace, "c");
	auto promiseD = nested(trace, "d");
	auto promise = bzd::waitAll(promiseA, promiseB, promiseC, promiseD);
	promise.sync();
	EXPECT_EQ(trace, "[a1][b1][c1][d1][a0][a2][b0][b2][c0][c2][d0][d2]");
}

bzd::Async<int, int> waitAllNested(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 5);
	auto promiseY = nested(trace, "y");
	auto promiseZ = nested(trace, "z");
	co_await bzd::waitAll(promiseY, promiseZ);
	appendToTrace(trace, id, 6);

	co_return 12;
}

TEST(Coroutine, waitAllNested)
{
	bzd::String<128> trace;
	auto promiseA = waitAllNested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promise = bzd::waitAll(promiseA, promiseB);
	promise.sync();
	EXPECT_EQ(trace, "[a5][b3][b1][y1][z1][b0][b2][b4][b3][y0][y2][z0][z2][a6][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
}

TEST(Coroutine, waitAny)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promise = bzd::waitAny(promiseA, promiseB);
	bzd::ignore = promise.sync();
	std::cout << trace.data() << std::endl;
	EXPECT_EQ(trace, "[a1][b3][a0][a2]");
}

TEST(Coroutine, waitAnyMany)
{
	bzd::String<128> trace;
	auto promiseA = deepNested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promiseC = nested(trace, "c");
	auto promiseD = deepNested(trace, "d");
	auto promise = bzd::waitAny(bzd::move(promiseA), bzd::move(promiseB), bzd::move(promiseC), bzd::move(promiseD));
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a3][b3][c1][d3][a1][b1][c0][c2]");
}

bzd::Async<int, int> asyncAdd(int a, int b)
{
    co_return a + b;
}

bzd::Async<int, int> asyncFibonacci(int n)
{
    if (n <= 2)
        co_return 1;

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

TEST(Coroutine, fibonacci)
{
	auto fibonacci1 = asyncFibonacci(12);
	auto fibonacci2 = asyncFibonacci(16);
	auto fibonacci3 = asyncFibonacci(18);
	auto fibonacci4 = asyncFibonacci(20);
	auto promise = bzd::waitAll(fibonacci1, fibonacci2, fibonacci3, fibonacci4);
	const auto result = promise.sync();
	EXPECT_EQ(result.size(), 4);
	EXPECT_EQ(result.get<0>().value(), 144);
	EXPECT_EQ(result.get<1>().value(), 987);
	EXPECT_EQ(result.get<2>().value(), 2584);
	EXPECT_EQ(result.get<3>().value(), 6765);
}
