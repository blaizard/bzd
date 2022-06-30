#include "cc/bzd/core/async.hh"
#include "cc/bzd/test/test.hh"

bzd::Async<bzd::UInt64> nopNoTrace(bzd::UInt64 retVal)
{
	co_return retVal;
}

bzd::Async<bzd::UInt64> loopSynchronously(int count)
{
	bzd::UInt64 result{0};
	for (int i = 0; i < count; ++i)
	{
		const auto output = co_await nopNoTrace(i);
		result += output.value();
	}
	co_return result;
}

TEST_ASYNC(Coroutine, Stackoverflow)
{
	const auto result = co_await loopSynchronously(100000);
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 4999950000ULL);
	co_return {};
}
