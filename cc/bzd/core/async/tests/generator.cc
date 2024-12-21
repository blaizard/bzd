#include "cc/bzd/container/string.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/move_only.hh"
#include "cc/bzd/utility/scope_guard.hh"

bzd::Generator<bzd::Size> generator(bzd::Size count)
{
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_yield i;
	}
}

TEST_ASYNC(Generator, OneCall)
{
	auto async = generator(10);

	auto maybeIt = co_await async.begin();
	EXPECT_TRUE(maybeIt);
	EXPECT_EQ(*maybeIt.value(), static_cast<bzd::Size>(0));

	co_return {};
}

TEST_ASYNC(Generator, TwoCalls)
{
	auto async = generator(10);

	auto maybeIt = co_await async.begin();
	EXPECT_TRUE(maybeIt);
	auto it = maybeIt.valueMutable();

	EXPECT_EQ(*it, static_cast<bzd::Size>(0));
	EXPECT_TRUE(co_await ++it);
	EXPECT_EQ(*it, static_cast<bzd::Size>(1));

	co_return {};
}

bzd::Async<bzd::Size> lazy(bzd::Size value) { co_return value; }

bzd::Generator<bzd::Size> generatorWithAwaitable(bzd::Size count)
{
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_yield co_await lazy(i);
	}
}

TEST_ASYNC(Generator, WithAwaitable)
{
	auto async = generatorWithAwaitable(10);

	auto maybeIt = co_await async.begin();
	EXPECT_TRUE(maybeIt);
	auto it = maybeIt.valueMutable();

	EXPECT_EQ(*it, static_cast<bzd::Size>(0));
	EXPECT_TRUE(co_await ++it);
	EXPECT_EQ(*it, static_cast<bzd::Size>(1));

	co_return {};
}

bzd::Generator<bzd::Size> generatorNested(bzd::Size count)
{
	auto async = generator(count);
	auto it = co_await !async.begin();
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_yield *it;
		co_await !++it;
	}
}

TEST_ASYNC(Generator, NestedGenerator)
{
	auto async = generatorNested(10);
	auto maybeIt = co_await async.begin();
	EXPECT_TRUE(maybeIt);
	auto it = maybeIt.valueMutable();

	EXPECT_EQ(*it, static_cast<bzd::Size>(0));
	EXPECT_TRUE(co_await ++it);
	EXPECT_EQ(*it, static_cast<bzd::Size>(1));

	co_return {};
}

bzd::Async<bzd::Size> generatorErrorAsync() { co_return bzd::error::Failure("Dummy"_csv); }

bzd::Generator<bzd::Size> generatorError(bzd::Size count)
{
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_await !generatorErrorAsync();
		co_yield i;
	}
}

TEST_ASYNC(Generator, ErrorPropagationWithinGenerator)
{
	auto async = generatorError(10);
	auto maybeIt = co_await async.begin();
	EXPECT_FALSE(maybeIt);

	co_return {};
}

TEST_ASYNC(Generator, ForLoop)
{
	auto async = generator(10);
	bzd::Size expected{0};
	co_await !bzd::async::forEach(bzd::move(async), [&](auto&& value) {
		EXPECT_EQ(value, expected);
		++expected;
		return true;
	});
	EXPECT_EQ(expected, 10u);

	co_return {};
}

TEST_ASYNC(Generator, Iterator)
{
	auto async = generator(10);
	bzd::Size expected{0};
	for (auto it = co_await !async.begin(); it != async.end();)
	{
		EXPECT_EQ(*it, expected);
		++expected;
		co_await !++it;
	}
	EXPECT_EQ(expected, 10u);

	co_return {};
}

TEST_ASYNC(Generator, Empty)
{
	auto async = generator(0u);
	auto it = co_await !async.begin();
	EXPECT_EQ(it, async.end());

	co_return {};
}
