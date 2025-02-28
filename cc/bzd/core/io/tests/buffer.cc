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
		auto maybeResult = sink.tryGetNew();
		ASSERT_TRUE(maybeResult);
		ASSERT_EQ(maybeResult.value(), 12);
	}
	{
		auto maybeResult = sink.tryGetNew();
		ASSERT_FALSE(maybeResult);
	}

	auto sink2 = buffer.makeSink();
	{
		auto maybeResult = sink2.tryGetNew();
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
		auto maybeResult = sink.tryGetNew(2);
		ASSERT_FALSE(maybeResult);
	}

	ASSERT_TRUE(source.trySet(13));

	{
		auto maybeResult = sink.tryGetNew(2);
		ASSERT_TRUE(maybeResult);
		const auto expected = {12, 13};
		EXPECT_EQ_RANGE(maybeResult.value(), expected);
	}

	{
		auto maybeResult = sink.tryGetNew(2);
		ASSERT_FALSE(maybeResult);
	}

	ASSERT_TRUE(source.trySet(14));

	{
		auto maybeResult = sink.tryGetNew(2);
		ASSERT_TRUE(maybeResult);
		const auto expected = {13, 14};
		EXPECT_EQ_RANGE(maybeResult.value(), expected);
	}
}

TEST_ASYNC(Buffer, Async)
{
	bzd::io::Buffer<int, 10u, "my.id"> buffer{};
	auto source = buffer.makeSource();
	auto sink = buffer.makeSink();
	{
		const auto result = co_await bzd::async::any(sink.getNew(), bzd::test::delay(10));
		EXPECT_FALSE(result.template get<0>());
		EXPECT_TRUE(result.template get<1>());
	}
	co_await !source.set(12);
	{
		const auto result = co_await bzd::async::any(sink.getNew(), bzd::test::delay(10));
		EXPECT_TRUE(result.template get<0>());
		EXPECT_FALSE(result.template get<1>());
		EXPECT_EQ(result.template get<0>().value().value(), 12);
	}
	{
		const auto result = co_await bzd::async::any(sink.getNew(), bzd::test::delay(10));
		EXPECT_FALSE(result.template get<0>());
		EXPECT_TRUE(result.template get<1>());
	}
	{
		const auto result = co_await bzd::async::all(sink.getNew(), source.set(13));
		EXPECT_TRUE(result.template get<0>());
		EXPECT_TRUE(result.template get<1>());
		EXPECT_EQ(result.template get<0>().value(), 13);
	}
	{
		const auto result = co_await bzd::async::all(source.set(14), sink.getNew());
		EXPECT_TRUE(result.template get<0>());
		EXPECT_TRUE(result.template get<1>());
		EXPECT_EQ(result.template get<1>().value(), 14);
	}

	co_return {};
}

TEST_ASYNC(Buffer, Notification)
{
	bzd::io::Buffer<int, 10u, "my.id"> buffer{};
	auto source = buffer.makeSource();
	auto sink = buffer.makeSink();

	auto setWithDelay = [&source]() -> bzd::Async<> {
		co_await bzd::async::yield();
		co_await bzd::async::yield();
		co_await bzd::async::yield();
		co_await bzd::async::yield();
		// Test that the notification happens after setting the value.
		auto setter = co_await !source.set();
		co_await bzd::async::yield();
		setter.valueMutable() = 14;
		co_return {};
	};

	{
		const auto result = co_await bzd::async::all(sink.getNew(), setWithDelay());
		EXPECT_TRUE(result.template get<0>());
		EXPECT_TRUE(result.template get<1>());
		EXPECT_EQ(result.template get<0>().value(), 14);
	}

	co_return {};
}
