#include "cc/bzd/core/async.hh"
#include "cc/bzd/test/test.hh"

#include <iostream>

bzd::Async<int> generateError()
{
	co_return bzd::error(bzd::ErrorType::failure, CSTR("Dummy"));
}

bzd::Async<> generateSuccessVoid()
{
	co_return {};
}

bzd::Async<int> generateSuccessInt(const int value)
{
	co_return value;
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
		// const auto value = co_await generateError().assert();
		// EXPECT_EQ(value, 42);
	}
	co_return {};
}
