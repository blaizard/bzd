#include "cc/bzd/core/async.hh"
#include "cc/bzd/test/test.hh"

#include <iostream>

bzd::Async<int> generateError()
{
	co_return bzd::error(bzd::ErrorType::failure, "Dummy"_csv);
}

bzd::Async<> generateSuccessVoid()
{
	co_return {};
}

bzd::Async<int> generateSuccessInt(const int value)
{
	co_return value;
}

bzd::Async<> nestedError(int& step)
{
	++step;
	co_await generateError().assert();
	++step;
	co_return {};
}

bzd::Async<> deeplyNestedError(int& step)
{
	++step;
	co_await nestedError(step).assert();
	++step;
	co_return {};
}

TEST_ASYNC(Coroutine, Error)
{
	{
		const auto result = co_await generateError();
		EXPECT_FALSE(result);
	}
	{
		const auto value = co_await generateSuccessInt(42).assert();
		EXPECT_EQ(value, 42);
	}
	{
		int step{-1};
		const auto result = co_await nestedError(step);
		EXPECT_FALSE(result);
		EXPECT_STREQ(result.error().getMessage().data(), "Dummy");
		EXPECT_EQ(step, 0);
	}
	{
		int step{-1};
		const auto result = co_await deeplyNestedError(step);
		EXPECT_FALSE(result);
		EXPECT_STREQ(result.error().getMessage().data(), "Dummy");
		EXPECT_EQ(step, 1);
	}
	co_return {};
}
