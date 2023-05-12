#include "cc/bzd/core/async.hh"

#include "cc/bzd/container/string.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/move_only.hh"
#include "cc/bzd/utility/pattern/to_string/integral.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace {
void appendToTrace(bzd::interface::String& trace, bzd::StringView id, int checkpoint)
{
	trace += '[';
	trace += id;
	bzd::toString(trace.appender(), checkpoint);
	trace += ']';
	//::std::cout << "[" << id.data() << checkpoint << "]" << ::std::flush;
}
} // namespace

bzd::Async<> nopVoid(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, -1);
	co_return {};
}

TEST_ASYNC(Coroutine, Void)
{
	bzd::String<32> trace;
	co_await !nopVoid(trace, "a");
	EXPECT_EQ(trace, "[a-1]");
	co_return {};
}

TEST_ASYNC(Coroutine, CoAwaitFromVariable)
{
	bzd::String<32> trace;
	auto async = nopVoid(trace, "a");
	const auto result = co_await async;
	EXPECT_TRUE(result);
	EXPECT_EQ(trace, "[a-1]");
	co_return {};
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
	co_return {};
}

bzd::Async<int> nested(bzd::interface::String& trace, bzd::StringView id, int retVal = 42)
{
	appendToTrace(trace, id, 1);
	auto result = co_await nop(trace, id, retVal);
	co_await bzd::async::yield();
	appendToTrace(trace, id, 2);
	co_return result;
}

TEST_ASYNC(Coroutine, Nested)
{
	bzd::String<32> trace;
	co_await !nested(trace, "a");
	EXPECT_EQ(trace, "[a1][a0][a2]");
	co_return {};
}

bzd::Async<int> deepNested(bzd::interface::String& trace, bzd::StringView id)
{
	int result = 0;
	for (int i = 0; i < 3; ++i)
	{
		result += i;
		appendToTrace(trace, id, 3);
		co_await !nested(trace, id);
		appendToTrace(trace, id, 4);
	}
	co_return bzd::move(result);
}

TEST_ASYNC(Coroutine, DeepNested)
{
	bzd::String<128> trace;
	co_await !deepNested(trace, "a");
	EXPECT_EQ(trace, "[a3][a1][a0][a2][a4][a3][a1][a0][a2][a4][a3][a1][a0][a2][a4]");
	co_return {};
}

bzd::Async<int> passThrough(bzd::interface::String& trace, bzd::StringView id, int retVal) { return nested(trace, id, retVal); }

TEST_ASYNC(Coroutine, PassThrough)
{
	bzd::String<32> trace;
	const auto result = co_await passThrough(trace, "a", 34);
	EXPECT_EQ(trace, "[a1][a0][a2]");
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 34);
	co_return {};
}

TEST_ASYNC(Coroutine, asyncAll)
{
	bzd::String<128> trace;
	const auto result = co_await bzd::async::all(nested(trace, "a", 10), nested(trace, "b", -4));
	EXPECT_EQ(trace, "[a1][a0][b1][b0][a2][b2]");
	EXPECT_EQ(result.size(), 2U);
	EXPECT_TRUE(result.get<0>());
	EXPECT_EQ(result.get<0>().value(), 10);
	EXPECT_EQ(result.get<1>().value(), -4);
	co_return {};
}

TEST_ASYNC(Coroutine, asyncAllDifferent)
{
	bzd::String<128> trace;
	[[maybe_unused]] auto result = co_await bzd::async::all(nested(trace, "a"), deepNested(trace, "b"));
	EXPECT_EQ(trace, "[a1][a0][b3][b1][b0][a2][b2][b4][b3][b1][b0][b2][b4][b3][b1][b0][b2][b4]");
	co_return {};
}

TEST_ASYNC(Coroutine, asyncAllMany)
{
	bzd::String<128> trace;
	[[maybe_unused]] auto result = co_await bzd::async::all(nested(trace, "a"), nested(trace, "b"), nested(trace, "c"), nested(trace, "d"));
	EXPECT_EQ(trace, "[a1][a0][b1][b0][c1][c0][d1][d0][a2][b2][c2][d2]");
	co_return {};
}

bzd::Async<int> asyncAllNested(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 5);
	[[maybe_unused]] auto result = co_await bzd::async::all(nested(trace, "y"), nested(trace, "z"));
	appendToTrace(trace, id, 6);

	co_return 12;
}

TEST_ASYNC(Coroutine, asyncAllNested)
{
	bzd::String<128> trace;
	[[maybe_unused]] auto result = co_await bzd::async::all(asyncAllNested(trace, "a"), deepNested(trace, "b"));
	//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
	EXPECT_EQ(trace, "[a5][b3][b1][b0][y1][y0][z1][z0][b2][b4][b3][b1][b0][y2][z2][a6][b2][b4][b3][b1][b0][b2][b4]");
	co_return {};
}

TEST_ASYNC(Coroutine, asyncAny)
{
	bzd::String<128> trace;
	const auto result = co_await bzd::async::any(nested(trace, "a"), deepNested(trace, "b"));
	//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
	EXPECT_EQ(trace, "[a1][a0][b3][b1][b0][a2]");
	EXPECT_EQ(result.size(), 2U);
	EXPECT_TRUE(result.get<0>());
	EXPECT_FALSE(result.get<1>());
	EXPECT_EQ(result.get<0>().value().value(), 42);
	co_return {};
}

TEST_ASYNC(Coroutine, asyncAnyMany)
{
	bzd::String<128> trace;
	const auto result =
		co_await bzd::async::any(deepNested(trace, "a"), deepNested(trace, "b"), nested(trace, "c", -432), deepNested(trace, "d"));
	//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
	EXPECT_EQ(trace, "[a3][a1][a0][b3][b1][b0][c1][c0][d3][d1][d0][a2][a4][a3][a1][a0][b2][b4][b3][b1][b0][c2]");
	EXPECT_EQ(result.size(), 4U);
	EXPECT_FALSE(result.get<0>());
	EXPECT_FALSE(result.get<1>());
	EXPECT_TRUE(result.get<2>());
	EXPECT_FALSE(result.get<3>());
	EXPECT_EQ(result.get<2>().value().value(), -432);
	co_return {};
}

bzd::Async<> yieldLoop(bzd::interface::String& trace, bzd::StringView id, bzd::Size counter)
{
	while (counter--)
	{
		appendToTrace(trace, id, 0);
		co_await bzd::async::yield();
	}
	co_return {};
}

TEST_ASYNC(Coroutine, asyncAnyYield)
{
	bzd::String<128> trace;
	const auto result = co_await bzd::async::any(yieldLoop(trace, "a", 10), yieldLoop(trace, "b", 2));
	//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
	EXPECT_EQ(trace, "[a0][b0][a0][b0][a0]");
	EXPECT_EQ(result.size(), 2U);
	co_return {};
}

bzd::Async<int> anyNested(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 6);
	const auto result = co_await bzd::async::any(deepNested(trace, "b"), nested(trace, "c", 42));
	appendToTrace(trace, id, 7);

	co_return result.get<1>().value().value();
}

TEST_ASYNC(Coroutine, asyncAnyNested)
{
	{
		bzd::String<128> trace;
		co_await !anyNested(trace, "a");
		//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
		EXPECT_EQ(trace, "[a6][b3][b1][b0][c1][c0][b2][b4][b3][b1][b0][c2][a7]");
	}
	{
		bzd::String<128> trace;
		[[maybe_unused]] const auto result = co_await bzd::async::any(nested(trace, "d"), anyNested(trace, "a"));
		//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
		EXPECT_EQ(trace, "[d1][d0][a6][d2]");
	}
	{
		bzd::String<128> trace;
		[[maybe_unused]] const auto result =
			co_await bzd::async::any(bzd::async::any(anyNested(trace, "d"), anyNested(trace, "a")), nested(trace, "c"));
		//::std::cout << ::std::endl << "HERE: " << trace.data() << ::std::endl;
		EXPECT_EQ(trace, "[c1][c0][d6][a6][c2]");
	}

	co_return {};
}

bzd::Async<> asyncDestroyMonitor(bzd::interface::String& trace, bzd::StringView id, bzd::Size& destroyedCounter)
{
	bzd::ScopeGuard scope{[&destroyedCounter]() { ++destroyedCounter; }};
	appendToTrace(trace, id, 0);
	co_await bzd::async::yield();
	appendToTrace(trace, id, 1);
	co_await bzd::async::yield();
	appendToTrace(trace, id, 2);
	co_await bzd::async::yield();
	appendToTrace(trace, id, 3);

	co_return {};
}

TEST_ASYNC(Coroutine, asyncAnyDestroy)
{
	bzd::String<128> trace;
	bzd::Size destroyedCounter{0U};
	[[maybe_unused]] const auto result =
		co_await bzd::async::any(yieldLoop(trace, "a", 2), asyncDestroyMonitor(trace, "b", destroyedCounter));
	EXPECT_EQ(trace, "[a0][b0][a0][b1]");
	EXPECT_EQ(destroyedCounter, 1U);

	co_return {};
}

bzd::Async<> asyncSuspend(bzd::interface::String& trace, bzd::StringView id)
{
	appendToTrace(trace, id, 0);
	bzd::async::ExecutableSuspended suspended{};
	co_await bzd::async::suspend([&](auto&& executable) { suspended.own(bzd::move(executable)); });
	appendToTrace(trace, id, 1);

	co_return {};
}

TEST_ASYNC(Coroutine, asyncAnySuspend)
{
	bzd::String<128> trace;
	[[maybe_unused]] const auto result = co_await bzd::async::any(yieldLoop(trace, "a", 2), asyncSuspend(trace, "b"));
	EXPECT_EQ(trace, "[a0][b0][a0]");
	EXPECT_TRUE(result.get<0>());
	EXPECT_FALSE(result.get<1>());

	co_return {};
}

bzd::Async<int> asyncAdd(int a, int b) { co_return a + b; }

bzd::Async<int> asyncFibonacci(int n)
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

TEST_ASYNC(Coroutine, fibonacci)
{
	const auto result = co_await bzd::async::all(asyncFibonacci(12), asyncFibonacci(16), asyncFibonacci(18), asyncFibonacci(20));
	EXPECT_EQ(result.size(), 4U);
	EXPECT_EQ(result.get<0>().value(), 144);
	EXPECT_EQ(result.get<1>().value(), 987);
	EXPECT_EQ(result.get<2>().value(), 2584);
	EXPECT_EQ(result.get<3>().value(), 6765);
	co_return {};
}

bzd::Async<bzd::test::MoveOnly> moveOnlyObjectNested(bzd::Bool* destroyedBeforeMoved)
{
	bzd::test::MoveOnly obj{destroyedBeforeMoved};
	co_return bzd::move(obj);
}

bzd::Async<bzd::test::MoveOnly> moveOnlyObject(bzd::Bool* destroyedBeforeMoved)
{
	co_return co_await moveOnlyObjectNested(destroyedBeforeMoved);
}

TEST_ASYNC(Coroutine, MoveOnly)
{
	bzd::Bool destroyedBeforeMoved{false};
	{
		const auto result = co_await moveOnlyObject(&destroyedBeforeMoved);
		EXPECT_TRUE(result);
		EXPECT_FALSE(destroyedBeforeMoved);
		EXPECT_GE(result->getMovedCounter(), 1U);
	}
	EXPECT_TRUE(destroyedBeforeMoved);

	co_return {};
}

bzd::Async<bzd::test::MoveOnly> moveOnlyObjectWithTry(bzd::Bool* destroyedBeforeMoved)
{
	auto result = co_await !moveOnlyObjectNested(destroyedBeforeMoved);
	EXPECT_FALSE(*destroyedBeforeMoved);
	co_return bzd::move(result);
}

TEST_ASYNC(Coroutine, MoveOnlyWithTry)
{
	bzd::Bool destroyedBeforeMoved{false};
	{
		const auto result = co_await moveOnlyObjectWithTry(&destroyedBeforeMoved);
		EXPECT_TRUE(result);
		EXPECT_FALSE(destroyedBeforeMoved);
		EXPECT_GE(result->getMovedCounter(), 1U);
	}
	EXPECT_TRUE(destroyedBeforeMoved);

	co_return {};
}
