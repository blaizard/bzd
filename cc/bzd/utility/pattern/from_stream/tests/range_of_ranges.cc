#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

#define AllTestIChannel (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks)

TEST_ASYNC(PatternFromStream, RangeOdRanges, AllTestIChannel)
{
	const auto keywords = {
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
		bzd::ToRangeOfRanges wrapper{keywords};
		const auto size = co_await !bzd::fromStream(channel.reader(), wrapper);
		EXPECT_EQ(size, bzd::size(exactMatch));
	}

	co_return {};
}
