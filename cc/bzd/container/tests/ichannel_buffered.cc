#include "cc/bzd/container/ichannel_buffered.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;

TEST_ASYNC(IChannelBuffered, Reader, (TestIChannel, TestIChannelZeroCopy))
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};
	EXPECT_EQ(channel.size(), 0u);

	// Empty ichannel
	{
		auto maybeScope = co_await channel.reader();
		EXPECT_FALSE(maybeScope);
		EXPECT_EQ(channel.size(), 0u);
	}

	// Add content to ichannel
	in << "abcdef";
	{
		auto scope = co_await !channel.reader();
		EXPECT_EQ(channel.size(), 0u);
		auto it = scope.begin();
		EXPECT_EQ(*it, 'a');
		++it;
		EXPECT_EQ(*it, 'b');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 4u);

	// Consume from the inner buffer
	{
		auto scope = co_await !channel.reader();
		EXPECT_EQ(channel.size(), 0u);
		auto it = scope.begin();
		EXPECT_EQ(*it, 'c');
		++it;
		EXPECT_EQ(*it, 'd');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 2u);

	// Consume from the inner buffer
	{
		auto scope = co_await !channel.reader();
		EXPECT_EQ(channel.size(), 0u);
		auto it = scope.begin();
		EXPECT_EQ(*it, 'e');
		++it;
		EXPECT_EQ(*it, 'f');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 0u);

	// Consume from the new buffer
	in << "ghi";
	{
		auto scope = co_await !channel.reader();
		EXPECT_EQ(channel.size(), 0u);
		auto it = scope.begin();
		EXPECT_EQ(*it, 'g');
		++it;
		EXPECT_EQ(*it, 'h');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 1u);

	co_return {};
}

/*
TEST_ASYNC(IChannelBuffered, Read, (TestIChannel))
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};
	EXPECT_EQ(channel.size(), 0u);

	// Empty ichannel
	{
		auto maybeScope = co_await channel.read(12u);
		EXPECT_FALSE(maybeScope);
		EXPECT_EQ(channel.size(), 0u);
	}

	co_return {};
}
*/