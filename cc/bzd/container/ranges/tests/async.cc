#include "cc/bzd/container/ranges/async.hh"

#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/container/ranges/views/views.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

TEST_ASYNC(AsyncRange, Simple, (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks))
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};

	in << "ABCDEF";
	{
		auto generator = channel.reader();
		auto view = generator | bzd::ranges::transform([](const auto c) { return c + ('a' - 'A'); });
		auto maybeRange = co_await view;
		auto it = maybeRange->begin();
		EXPECT_EQ(*it, 'a');
		++it;
	}
	{
		auto generator = channel.reader();
		auto maybeRange = co_await generator;
		auto it = maybeRange->begin();
		EXPECT_EQ(*it, 'B');
		++it;
	}

	co_return {};
}
