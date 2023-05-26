#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"

using TestIChannel = bzd::test::IChannel<char, 32u, false>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, true>;

TEST_ASYNC(PatternFromStream, NoArguments, (TestIChannel, TestIChannelZeroCopy))
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
