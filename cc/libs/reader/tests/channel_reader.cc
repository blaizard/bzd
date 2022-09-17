#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/test/types/istream_reader.hh"
#include "cc/libs/reader/stream_reader.hh"

TEST_ASYNC(ChannelReader, ReadUntil)
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

	in << "abcdefghijklmnopqrstuvwxyz";
	{
		auto generator = reader.readUntil('z');
		const auto string = co_await !bzd::make<bzd::String<40>>(bzd::move(generator));
		EXPECT_EQ(string.size(), 26u);
		EXPECT_EQ(string, "dabcdefghijklmnopqrstuvwxy");
	}

	co_return {};
}
