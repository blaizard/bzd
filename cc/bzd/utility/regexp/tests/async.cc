#include "cc/bzd/utility/regexp/async.hh"

#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"

using TestIChannel = bzd::test::IChannel<char, 32u, false>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, true>;

TEST_ASYNC(RegexpAsync, String, (TestIChannel, TestIChannelZeroCopy))
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};
	in << "abcdef";

	{
		const auto size = co_await !bzd::RegexpAsync{"abc"}.match(channel);
		EXPECT_EQ(size, 3u);
	}

	{
		const auto size = co_await !bzd::RegexpAsync{"d"}.match(channel);
		EXPECT_EQ(size, 1u);
	}

	{
		const auto result = co_await bzd::RegexpAsync{"f"}.match(channel);
		EXPECT_FALSE(result);
	}

	{
		const auto size = co_await !bzd::RegexpAsync{"ef"}.match(channel);
		EXPECT_EQ(size, 2u);
	}

	{
		const auto result = co_await bzd::RegexpAsync{"f"}.match(channel);
		EXPECT_FALSE(result);
	}

	in << "ghi";
	{
		const auto size = co_await !bzd::RegexpAsync{"gh"}.match(channel);
		EXPECT_EQ(size, 2u);
	}
	in << "jkl";
	{
		const auto size = co_await !bzd::RegexpAsync{"ijkl"}.match(channel);
		EXPECT_EQ(size, 4u);
	}
	in << "mno";
	{
		const auto result = co_await bzd::RegexpAsync{"mnop"}.match(channel);
		EXPECT_FALSE(result);
	}

	co_return {};
}

TEST_ASYNC(RegexpAsync, SplitChannel, (TestIChannel, TestIChannelZeroCopy))
{
	TestType in{};
	bzd::IChannelBuffered<char, 4u> channel{in};

	in << "abc";
	{
		const auto size = co_await !bzd::RegexpAsync{"abc"}.match(channel);
		EXPECT_EQ(size, 3u);
	}
	in << "defg";
	{
		const auto size = co_await !bzd::RegexpAsync{"defg"}.match(channel);
		EXPECT_EQ(size, 4u);
	}

	co_return {};
}

TEST_ASYNC(RegexpAsync, Features, (TestIChannel, TestIChannelZeroCopy))
{
	TestType in{};
	bzd::IChannelBuffered<char, 8u> channel{in};

	{
		in << "a*c";
		const auto size = co_await !bzd::RegexpAsync{"a\\*c"}.match(channel);
		EXPECT_EQ(size, 3u);
	}

	{
		in << " \t\na";
		const auto size = co_await !bzd::RegexpAsync{"\\s+a"}.match(channel);
		EXPECT_EQ(size, 4u);
	}

	{
		in << "agf";
		const auto size = co_await !bzd::RegexpAsync{"a[^bd]f"}.match(channel);
		EXPECT_EQ(size, 3u);
	}

	{
		in << "a012345c";
		const auto size = co_await !bzd::RegexpAsync{"a[0-9]*c"}.match(channel);
		EXPECT_EQ(size, 8u);
	}

	{
		in << "a012345c";
		const auto result = co_await bzd::RegexpAsync{"a[0-9]*d"}.match(channel);
		EXPECT_FALSE(result);
	}

	co_return {};
}

TEST_ASYNC(RegexpAsync, Capture, (TestIChannel, TestIChannelZeroCopy))
{
	TestType in{};
	bzd::IChannelBuffered<char, 8u> channel{in};

	{
		in << "a*c";
		bzd::String<12u> string;
		const auto size = co_await !bzd::RegexpAsync{"a\\*c"}.capture(channel, string.assigner());
		EXPECT_EQ(size, 3u);
		EXPECT_STREQ("a*c", string.data());
	}

	{
		in << "a1239c";
		bzd::String<12u> string;
		const auto size = co_await !bzd::RegexpAsync{"a[0-9]+c"}.capture(channel, string.assigner());
		EXPECT_EQ(size, 6u);
		EXPECT_STREQ("a1239c", string.data());
	}

	{
		in << "a1239";

		bzd::String<12> string;
		const auto result = co_await bzd::RegexpAsync{"a[0-9]+c"}.capture(channel, string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("a1239", string.data());
	}

	// String too small but match.
	{
		in << "1239";

		bzd::String<2> string;
		const auto result = co_await bzd::RegexpAsync{"[0-9]+"}.capture(channel, string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("12", string.data());
	}

	// String too small no match.
	{
		in << "1239";

		bzd::String<4> string;
		const auto result = co_await bzd::RegexpAsync{"[0-9]+a"}.capture(channel, string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("1239", string.data());
	}

	co_return {};
}
