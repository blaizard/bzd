#include "cc/bzd/core/async.hh"

#include "cc/bzd/container/string.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/format/integral.hh"

#include <iostream>

namespace {
constexpr void appendToTrace(bzd::interface::String& trace, bzd::StringView id, int checkpoint)
{
	trace += '[';
	trace += id;
	toString(trace, checkpoint);
	trace += ']';
}
} // namespace

bzd::Async<void> nopVoid(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, -1);
	co_return;
}

TEST_ASYNC(Coroutine, Void)
{
	bzd::String<32> trace;
	co_await nopVoid(trace, "a");
	EXPECT_EQ(trace, "[a-1]");
}

bzd::Async<int> nop(bzd::interface::String& trace, bzd::StringView id, int retVal)
{
	appendToTrace(trace, id, 0);
	co_return retVal;
}

TEST_ASYNC(Coroutine, Base)
{
	bzd::String<32> trace;
	const auto result = co_await nop(trace, "a", 42);
	EXPECT_EQ(trace, "[a0]");
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 42);
}

bzd::Async<int> nested(bzd::interface::String& trace, bzd::StringView id, int retVal = 42)
{
	appendToTrace(trace, id, 1);
	auto result = co_await nop(trace, id, retVal);
	appendToTrace(trace, id, 2);
	co_return result;
}

TEST_ASYNC(Coroutine, Nested)
{
	bzd::String<32> trace;
	co_await nested(trace, "a");
	EXPECT_EQ(trace, "[a1][a0][a2]");
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

TEST_ASYNC(Coroutine, DeepNested)
{
	bzd::String<128> trace;
	co_await deepNested(trace, "a");
	EXPECT_EQ(trace, "[a3][a1][a0][a2][a4][a3][a1][a0][a2][a4][a3][a1][a0][a2][a4]");
}

bzd::Async<int> passThrough(bzd::interface::String& trace, bzd::StringView id, int retVal)
{
	return nested(trace, id, retVal);
}

TEST_ASYNC(Coroutine, PassThrough)
{
	bzd::String<32> trace;
	const auto result = co_await passThrough(trace, "a", 34);
	EXPECT_EQ(trace, "[a1][a0][a2]");
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 34);
}

bzd::Async<bzd::Executor*> fetchExecutor()
{
	auto exec = co_await bzd::async::getExecutor();
	co_return exec;
}

TEST(Coroutine, Executor)
{
	bzd::Executor executor;
	auto promise = fetchExecutor();
	const auto result = promise.run(executor);
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), &executor);
}

TEST_ASYNC(Coroutine, asyncAll)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a", 10);
	auto promiseB = nested(trace, "b", -4);
	const auto result = co_await bzd::async::all(promiseA, promiseB);
	EXPECT_EQ(trace, "[a1][b1][a0][a2][b0][b2]");
	EXPECT_EQ(result.size(), 2U);
	EXPECT_TRUE(result.get<0>());
	EXPECT_EQ(result.get<0>().value(), 10);
	EXPECT_EQ(result.get<1>().value(), -4);
}

TEST_ASYNC(Coroutine, asyncAllDifferent)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	co_await bzd::async::all(promiseA, promiseB);
	EXPECT_EQ(trace, "[a1][b3][a0][a2][b1][b0][b2][b4][b3][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
}

TEST_ASYNC(Coroutine, asyncAllMany)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = nested(trace, "b");
	auto promiseC = nested(trace, "c");
	auto promiseD = nested(trace, "d");
	co_await bzd::async::all(promiseA, promiseB, promiseC, promiseD);
	EXPECT_EQ(trace, "[a1][b1][c1][d1][a0][a2][b0][b2][c0][c2][d0][d2]");
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

TEST_ASYNC(Coroutine, asyncAllNested)
{
	bzd::String<128> trace;
	auto promiseA = asyncAllNested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	co_await bzd::async::all(promiseA, promiseB);
	EXPECT_EQ(trace, "[a5][b3][b1][y1][z1][b0][b2][b4][b3][y0][y2][z0][z2][a6][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
}

TEST_ASYNC(Coroutine, asyncAny)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	const auto result = co_await bzd::async::any(promiseA, promiseB);
	EXPECT_EQ(trace, "[a1][b3][a0][a2]");
	EXPECT_EQ(result.size(), 2U);
	EXPECT_TRUE(result.get<0>());
	EXPECT_FALSE(result.get<1>());
	EXPECT_EQ(result.get<0>().value().value(), 42);
}

TEST_ASYNC(Coroutine, asyncAnyMany)
{
	bzd::String<128> trace;
	auto promiseA = deepNested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promiseC = nested(trace, "c", -432);
	auto promiseD = deepNested(trace, "d");
	const auto result = co_await bzd::async::any(bzd::move(promiseA), bzd::move(promiseB), bzd::move(promiseC), bzd::move(promiseD));
	EXPECT_EQ(trace, "[a3][b3][c1][d3][a1][b1][c0][c2]");
	EXPECT_EQ(result.size(), 4U);
	EXPECT_FALSE(result.get<0>());
	EXPECT_FALSE(result.get<1>());
	EXPECT_TRUE(result.get<2>());
	EXPECT_FALSE(result.get<3>());
	EXPECT_EQ(result.get<2>().value().value(), -432);
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

TEST_ASYNC(Coroutine, fibonacci)
{
	auto fibonacci1 = asyncFibonacci(12);
	auto fibonacci2 = asyncFibonacci(16);
	auto fibonacci3 = asyncFibonacci(18);
	auto fibonacci4 = asyncFibonacci(20);
	const auto result = co_await bzd::async::all(fibonacci1, fibonacci2, fibonacci3, fibonacci4);
	EXPECT_EQ(result.size(), 4U);
	EXPECT_EQ(result.get<0>().value(), 144);
	EXPECT_EQ(result.get<1>().value(), 987);
	EXPECT_EQ(result.get<2>().value(), 2584);
	EXPECT_EQ(result.get<3>().value(), 6765);
}
