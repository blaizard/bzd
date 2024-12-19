#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

#define AllTestIChannel (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks)

TEST_ASYNC(PatternFromStream, RangeOfRanges, AllTestIChannel)
{
	const auto keywords = {
		"ba"_sv,
		"eba"_sv,
		"h"_sv,
		"hello"_sv,
		"info"_sv,
		"infobar"_sv,
		"information"_sv,
		"noooo"_sv,
	};
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};

	// Exact matches.
	for (const auto& exactMatch : keywords)
	{
		in << exactMatch;
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		using Metadata = typename bzd::FromString<decltype(wrapper)>::Metadata;
		const auto size = co_await !bzd::fromStream(channel.reader(), wrapper, Metadata{Metadata::Mode::greedy});
		EXPECT_EQ(size, bzd::size(exactMatch));
		EXPECT_STREQ(wrapper.value()->data(), exactMatch.data());
	}

	{
		in << "heba";
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto size = co_await !bzd::fromStream(channel.reader(), wrapper);
		EXPECT_EQ(size, 1u);
		EXPECT_STREQ(wrapper.value()->data(), "h");
	}
	// The 'e' is consumed by the previous invocation because it uses a greedy algorithm.
	/*
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto size = co_await !bzd::fromStream(channel.reader(), wrapper);
		EXPECT_EQ(size, 3u);
		EXPECT_STREQ(wrapper.value()->data(), "ba");
	}
	*/

	co_return {};
}

TEST_ASYNC(PatternFromStream, RangeOfRangesReusing, AllTestIChannel)
{
	const auto keywords = {
		"ba"_sv,
		"eba"_sv,
		"h"_sv,
		"hello"_sv,
		"info"_sv,
		"infobar"_sv,
		"information"_sv,
		"noooo"_sv,
	};
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};

	// re-using wrapper.
	{
		in << "helloinfobar";
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto result1 = co_await bzd::fromStream(channel.reader(), wrapper);
		EXPECT_TRUE(result1);
		EXPECT_EQ(result1.value(), 5u);
		const auto result2 = co_await bzd::fromStream(channel.reader(), wrapper);
		EXPECT_TRUE(result2);
		EXPECT_EQ(result2.value(), 7u);
	}

	co_return {};
}
