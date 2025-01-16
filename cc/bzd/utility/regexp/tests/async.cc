#include "cc/bzd/utility/regexp/async.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/ranges/views_async/join.hh"
#include "cc/bzd/utility/ranges/views_async/views_async.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

#define AllTestIChannel (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks)

TEST_ASYNC(RegexpAsync, String, AllTestIChannel)
{
	TestType in{};
	bzd::Array<char, 16u> buffer{};

	{
		auto reader = in.reader(buffer.asSpan());
		in << "abcdef";
		{
			const auto size = co_await !bzd::RegexpAsync{"abc"}.match(reader | bzd::ranges::join());
			EXPECT_EQ(size, 3u);
		}

		{
			const auto size = co_await !bzd::RegexpAsync{"d"}.match(reader | bzd::ranges::join());
			EXPECT_EQ(size, 1u);
		}

		{
			const auto result = co_await bzd::RegexpAsync{"f"}.match(reader | bzd::ranges::join());
			EXPECT_FALSE(result);
		}

		{
			const auto size = co_await !bzd::RegexpAsync{"ef"}.match(reader | bzd::ranges::join());
			EXPECT_EQ(size, 2u);
		}

		{
			const auto result = co_await bzd::RegexpAsync{"f"}.match(reader | bzd::ranges::join());
			EXPECT_FALSE(result);
		}
	}

	{
		auto reader = in.reader(buffer.asSpan());
		in << "ghi";
		{
			const auto size = co_await !bzd::RegexpAsync{"gh"}.match(reader | bzd::ranges::join());
			EXPECT_EQ(size, 2u);
		}
		in << "jkl";
		{
			const auto maybeSize = co_await bzd::RegexpAsync{"ijkl"}.match(reader | bzd::ranges::join());
			ASSERT_ASYNC_TRUE(maybeSize);
			EXPECT_EQ(maybeSize.value(), 4u);
		}
	}
	in << "mno";
	{
		const auto result = co_await bzd::RegexpAsync{"mnop"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_FALSE(result);
	}

	co_return {};
}

TEST_ASYNC(RegexpAsync, SplitChannel, AllTestIChannel)
{
	TestType in{};
	bzd::Array<char, 16u> buffer{};

	in << "abc";
	{
		const auto size = co_await !bzd::RegexpAsync{"abc"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 3u);
	}
	in << "defg";
	{
		const auto size = co_await !bzd::RegexpAsync{"defg"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 4u);
	}

	co_return {};
}

TEST_ASYNC(RegexpAsync, Features, AllTestIChannel)
{
	TestType in{};
	bzd::Array<char, 16u> buffer{};

	{
		in << "a*c";
		const auto size = co_await !bzd::RegexpAsync{"a\\*c"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 3u);
	}

	{
		in << " \t\na";
		const auto size = co_await !bzd::RegexpAsync{"\\s+a"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 4u);
	}

	{
		in << "agf";
		const auto size = co_await !bzd::RegexpAsync{"a[^bd]f"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 3u);
	}

	{
		in << "a012345c";
		const auto size = co_await !bzd::RegexpAsync{"a[0-9]*c"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 8u);
	}

	{
		auto range = in.reader(buffer.asSpan());
		in << "a012345c";
		const auto result = co_await bzd::RegexpAsync{"a[0-9]*d"}.match(range | bzd::ranges::join());
		EXPECT_FALSE(result);
		// Flush the rest.
		const auto size = co_await !bzd::RegexpAsync{".*"}.match(range | bzd::ranges::join());
		EXPECT_EQ(size, 1u);
	}

	{
		in << "012345";
		const auto size = co_await !bzd::RegexpAsync{"[0-9]+"}.match(in.reader(buffer.asSpan()) | bzd::ranges::join());
		EXPECT_EQ(size, 6u);
	}

	co_return {};
}

TEST_ASYNC(RegexpAsync, Capture, AllTestIChannel)
{
	TestType in{};
	bzd::Array<char, 8u> buffer{};

	{
		in << "a*c";
		bzd::String<12u> string;
		const auto size = co_await !bzd::RegexpAsync{"a\\*c"}.capture(in.reader(buffer.asSpan()) | bzd::ranges::join(), string.assigner());
		EXPECT_EQ(size, 3u);
		EXPECT_STREQ("a*c", string.data());
	}

	{
		in << "a1239c";
		bzd::String<12u> string;
		const auto size =
			co_await !bzd::RegexpAsync{"a[0-9]+c"}.capture(in.reader(buffer.asSpan()) | bzd::ranges::join(), string.assigner());
		EXPECT_EQ(size, 6u);
		EXPECT_STREQ("a1239c", string.data());
	}

	{
		in << "a1239";

		bzd::String<12> string;
		const auto result =
			co_await bzd::RegexpAsync{"a[0-9]+c"}.capture(in.reader(buffer.asSpan()) | bzd::ranges::join(), string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("a1239", string.data());
	}

	{
		in << "1239";

		bzd::String<10> string;
		const auto size = co_await !bzd::RegexpAsync{"[0-9]+"}.capture(in.reader(buffer.asSpan()) | bzd::ranges::join(), string.assigner());
		EXPECT_EQ(size, 4u);
		EXPECT_STREQ("1239", string.data());
	}

	// String too small but match.
	{
		in << "1239";

		bzd::String<2> string;
		const auto result =
			co_await bzd::RegexpAsync{"[0-9]+"}.capture(in.reader(buffer.asSpan()) | bzd::ranges::join(), string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("12", string.data());
	}

	// String too small no match.
	{
		in << "1239";

		bzd::String<4> string;
		const auto result =
			co_await bzd::RegexpAsync{"[0-9]+a"}.capture(in.reader(buffer.asSpan()) | bzd::ranges::join(), string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("1239", string.data());
	}

	co_return {};
}
