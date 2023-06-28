#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

#define AllTestIChannel (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks)

TEST_ASYNC(PatternFromStream, NoArguments, AllTestIChannel)
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};

	in << "Hello World";
	const auto result = co_await bzd::fromStream(channel, "Hello"_csv);
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 5u);

	const auto result2 = co_await bzd::fromStream(channel, "World"_csv);
	EXPECT_FALSE(result2);

	const auto result3 = co_await bzd::fromStream(channel, "\\s+World"_csv);
	EXPECT_TRUE(result3);
	EXPECT_EQ(result3.value(), 6u);

	co_return {};
}

TEST_ASYNC(PatternFromStream, StringArgument, AllTestIChannel)
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};

	in << "Hello World";
	{
		bzd::String<12u> world;
		const auto size = co_await !bzd::fromStream(channel, "Hello {}"_csv, world.assigner());
		EXPECT_EQ(size, 11u);
		EXPECT_STREQ(world.data(), "World");
	}

	in << "1 23456789abcd";
	{
		bzd::String<12u> numbers;
		const auto size = co_await !bzd::fromStream(channel, "[0-9]+ {:[0-9]+}"_csv, numbers.assigner());
		EXPECT_EQ(size, 10u);
		EXPECT_STREQ(numbers.data(), "23456789");
	}

	// Read the rest.
	{
		bzd::String<12u> rest;
		const auto size = co_await !bzd::fromStream(channel, "{:.*}"_csv, rest.assigner());
		EXPECT_EQ(size, 4u);
		EXPECT_STREQ(rest.data(), "abcd");
	}

	// String too short.
	in << "Hello Mister";
	{
		bzd::String<2u> string;
		const auto result = co_await bzd::fromStream(channel, "{:.*}"_csv, string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ(string.data(), "He");
	}

	co_return {};
}
