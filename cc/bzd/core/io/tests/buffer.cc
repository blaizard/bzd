#include "cc/bzd/core/io/buffer.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/utility.hh"

TEST(Buffer, Simple)
{
	bzd::io::Buffer<int, 10u, "my.id"> buffer{};
	auto source = buffer.makeSource();
	auto sink = buffer.makeSink();

	ASSERT_TRUE(source.trySet(12));
	ASSERT_STREQ(source.getName().data(), "my.id");
	ASSERT_STREQ(sink.getName().data(), "my.id");

	{
		auto maybeResult = sink.tryGet();
		ASSERT_TRUE(maybeResult);
		ASSERT_EQ(maybeResult.value(), 12);
	}
	{
		auto maybeResult = sink.tryGet();
		ASSERT_FALSE(maybeResult);
	}

	auto sink2 = buffer.makeSink();
	{
		auto maybeResult = sink2.tryGet();
		ASSERT_TRUE(maybeResult);
		ASSERT_EQ(maybeResult.value(), 12);
	}
}

TEST(Buffer, Range)
{
	bzd::io::Buffer<int, 10u, "my.id"> buffer{};
	auto source = buffer.makeSource();
	auto sink = buffer.makeSink();

	ASSERT_TRUE(source.trySet(12));

	{
		auto maybeResult = sink.tryGet(2);
		ASSERT_FALSE(maybeResult);
	}

	ASSERT_TRUE(source.trySet(13));

	{
		auto maybeResult = sink.tryGet(2);
		ASSERT_TRUE(maybeResult);
		const auto expected = {12, 13};
		const auto isEqual = bzd::algorithm::equal(maybeResult.value(), expected);
		EXPECT_TRUE(isEqual);
	}

	{
		auto maybeResult = sink.tryGet(2);
		ASSERT_FALSE(maybeResult);
	}

	ASSERT_TRUE(source.trySet(14));

	{
		auto maybeResult = sink.tryGet(2);
		ASSERT_TRUE(maybeResult);
		const auto expected = {13, 14};
		const auto isEqual = bzd::algorithm::equal(maybeResult.value(), expected);
		EXPECT_TRUE(isEqual);
	}
}

TEST_ASYNC(Buffer, Async)
{
	bzd::io::Buffer<int, 10u, "my.id"> buffer{};
	auto source = buffer.makeSource();
	auto sink = buffer.makeSink();

	{
		const auto result = co_await bzd::async::any(sink.get(), bzd::test::delay(10));
		EXPECT_FALSE(result.get<0>());
		EXPECT_TRUE(result.get<1>());
	}
	co_await !source.set(12);
	{
		const auto result = co_await bzd::async::any(sink.get(), bzd::test::delay(10));
		EXPECT_TRUE(result.get<0>());
		EXPECT_FALSE(result.get<1>());
		EXPECT_EQ(result.get<0>().value().value(), 12);
	}
	{
		const auto result = co_await bzd::async::any(sink.get(), bzd::test::delay(10));
		EXPECT_FALSE(result.get<0>());
		EXPECT_TRUE(result.get<1>());
	}
	{
		const auto result = co_await bzd::async::all(sink.get(), source.set(13));
		EXPECT_TRUE(result.get<0>());
		EXPECT_TRUE(result.get<1>());
		EXPECT_EQ(result.get<0>().value(), 13);
	}
	{
		const auto result = co_await bzd::async::all(sink.get(), source.set(14));
		EXPECT_TRUE(result.get<0>());
		EXPECT_TRUE(result.get<1>());
		EXPECT_EQ(result.get<0>().value(), 14);
	}

	co_return {};
}