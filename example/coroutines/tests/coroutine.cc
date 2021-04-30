#include "example/coroutines/async.h"
#include "bzd/container/string.h"
#include "bzd/utility/format/integral.h"

#include "cc_test/test.h"

#include <iostream>

namespace
{
constexpr void appendToTrace(bzd::interface::String& trace, bzd::StringView id, int checkpoint)
{
  trace += '[';
  trace += id;
  bzd::format::appendToString(trace, checkpoint);
  trace += ']';
}
}

bzd::Async nop(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 0);
	co_return bzd::makeError(12);
}

TEST(Coroutine, Base)
{
	bzd::String<32> trace;
	auto promise = nop(trace, "a");
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a0]");
}

bzd::Async nested(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 1);
	const auto result = co_await nop(trace, id);
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

bzd::Async deepNested(bzd::interface::String& trace, bzd::StringView id)
{
	int result = 0;
	for (int i = 0; i<3; ++i)
	{
		result += i;
		appendToTrace(trace, id, 3);
		co_await nested(trace, id);
		appendToTrace(trace, id, 4);
	}
	co_return result;
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
	auto promiseA = nested(trace, "a");
	auto promiseB = nested(trace, "b");
	auto promise = bzd::waitAll(promiseA, promiseB);
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][b1][a0][a2][b0][b2]");
}

TEST(Coroutine, waitAllDifferent)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promise = bzd::waitAll(promiseA, promiseB);
	bzd::ignore = promise.sync();
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
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][b1][c1][d1][a0][a2][b0][b2][c0][c2][d0][d2]");
}

bzd::Async waitAllNested(bzd::interface::String& trace, bzd::StringView id)
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
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a5][b3][b1][y1][z1][b0][b2][b4][b3][y0][y2][z0][z2][a6][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
}

TEST(Coroutine, waitAny)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = nested(trace, "b");
	auto promise = bzd::waitAny(promiseA, promiseB);
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][b1][a0][a2]");
}
