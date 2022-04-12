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

bzd::Async<int> generateSuccessIntWithDelay(const int value)
{
	co_await bzd::async::yield();
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

bzd::Async<> nestedErrorAny(int& step)
{
	++step;
	co_await bzd::async::any(generateError(), generateSuccessInt(36)).assert();
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

TEST_ASYNC(Coroutine, ErrorAny)
{
	{
		const auto value = co_await bzd::async::any(generateSuccessInt(12), generateSuccessInt(42)).assert();
		EXPECT_EQ(value, 12);
	}

	{
		const auto value = co_await bzd::async::any(generateSuccessIntWithDelay(12), generateSuccessInt(42)).assert<1>();
		EXPECT_EQ(value, 42);
	}

	{
		int step{-1};
		const auto result = co_await nestedErrorAny(step);
		EXPECT_FALSE(result);
		EXPECT_STREQ(result.error().getMessage().data(), "Dummy");
		EXPECT_EQ(step, 0);
	}

	co_return {};
}
