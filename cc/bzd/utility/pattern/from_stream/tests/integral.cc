#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"
#include "cc/bzd/utility/ranges/views_async/join.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

#define AllTestIChannel (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks)

TEST_ASYNC(PatternFromStream, Integral, AllTestIChannel)
{
	TestType in{};
	bzd::Array<char, 16u> buffer{};

	in << "42";
	{
		bzd::UInt16 value;
		const auto size = co_await !bzd::fromStream(in.reader(buffer.asSpan()) | bzd::ranges::join(), value);
		EXPECT_EQ(size, 2u);
		EXPECT_EQ(value, 42u);
	}

	in << "1234";
	{
		bzd::Int32 value;
		const auto size = co_await !bzd::fromStream(in.reader(buffer.asSpan()) | bzd::ranges::join(), value);
		EXPECT_EQ(size, 4u);
		EXPECT_EQ(value, 1234);
	}

	in << "-1234";
	{
		bzd::Int32 value;
		const auto size = co_await !bzd::fromStream(in.reader(buffer.asSpan()) | bzd::ranges::join(), value);
		EXPECT_EQ(size, 5u);
		EXPECT_EQ(value, -1234);
	}

	in << "42abcd";
	{
		bzd::Int8 value;
		const auto size = co_await !bzd::fromStream(in.reader(buffer.asSpan()) | bzd::ranges::join(), value);
		EXPECT_EQ(size, 2u);
		EXPECT_EQ(value, 42);
	}

	{
		bzd::Int8 value;
		const auto maybeSize = co_await bzd::fromStream(in.reader(buffer.asSpan()) | bzd::ranges::join(), value);
		EXPECT_FALSE(maybeSize);
	}

	co_return {};
}
