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
	co_return 42;
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

TEST(Coroutine, PromiseAnd)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = nested(trace, "b");
	auto promise = bzd::Async::promiseAnd(promiseA, promiseB);
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][b1][a0][a2][b0][b2]");
}

TEST(Coroutine, PromiseAndDifferent)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = deepNested(trace, "b");
	auto promise = bzd::Async::promiseAnd(promiseA, promiseB);
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][b3][a0][a2][b1][b0][b2][b4][b3][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
}

TEST(Coroutine, PromiseOr)
{
	bzd::String<128> trace;
	auto promiseA = nested(trace, "a");
	auto promiseB = nested(trace, "b");
	auto promise = bzd::Async::promiseOr(promiseA, promiseB);
	bzd::ignore = promise.sync();
	EXPECT_EQ(trace, "[a1][b1][a0][a2]");
}
