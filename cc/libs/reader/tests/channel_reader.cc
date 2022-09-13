#include "cc/libs/reader/stream_reader.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/test/types/istream_reader.hh"

TEST_ASYNC(StreamReader, ReadUntil)
{
	bzd::test::IChannel<char, 64u> in{};
	bzd::ChannelReader<char, 8u> reader{in};
	in << "hello";

	{
		const auto output = co_await reader.readUntil('l');
		EXPECT_TRUE(output);
		EXPECT_EQ(output.value(), "he"_sv);
	}

	{
		const auto output = co_await reader.readUntil('l');
		EXPECT_TRUE(output);
		EXPECT_EQ(output.value(), ""_sv);
	}

	{
		const auto output = co_await reader.readUntil('x');
		EXPECT_TRUE(output);
		EXPECT_EQ(output.value(), "llo"_sv);
	}

    in << "world"; // This should make the output split.

	{
		const auto output = co_await reader.readUntil('r');
		EXPECT_TRUE(output);
		EXPECT_EQ(output.value(), "wo"_sv);
	}

    // The buffer reached the border, it is splited.
	{
        auto generator = reader.readUntil('d');
		const auto output1 = co_await generator;
		EXPECT_TRUE(output1);
		EXPECT_EQ(output1.value(), "r"_sv);
		const auto output2 = co_await generator;
		EXPECT_TRUE(output2);
		EXPECT_EQ(output2.value(), "l"_sv);
	}

    co_return {};
}
