#include "cc/bzd/container/ichannel_buffered.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/test/types/ichannel_generator.hh"

using TestIChannel = bzd::test::IChannel<char, 32u>;
using TestIChannelZeroCopy = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelZeroCopyChunks = bzd::test::IChannel<char, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

TEST_ASYNC(IChannelBuffered, Reader, (TestIChannel, TestIChannelZeroCopy))
{
	TestType in{};
	bzd::IChannelBuffered<char, 16u> channel{in};
	EXPECT_EQ(channel.size(), 0u);

	// Empty ichannel
	{
		auto generator = channel.reader();
		auto maybeSuccess = co_await generator;
		EXPECT_FALSE(maybeSuccess);
		EXPECT_EQ(channel.size(), 0u);
	}

	// Add content to ichannel
	in << "abcdef";
	{
		auto generator = channel.reader();
		auto maybeRange = co_await generator;
		EXPECT_TRUE(maybeRange);
		EXPECT_EQ(channel.size(), 0u);
		auto it = maybeRange->begin();
		EXPECT_EQ(*it, 'a');
		++it;
		EXPECT_EQ(*it, 'b');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 4u);

	// Consume from the inner buffer
	{
		auto generator = channel.reader();
		auto maybeRange = co_await generator;
		EXPECT_EQ(channel.size(), 0u);
		auto it = maybeRange->begin();
		EXPECT_EQ(*it, 'c');
		++it;
		EXPECT_EQ(*it, 'd');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 2u);

	// Consume from the inner buffer
	{
		auto generator = channel.reader();
		auto maybeRange = co_await generator;
		EXPECT_EQ(channel.size(), 0u);
		auto it = maybeRange->begin();
		EXPECT_EQ(*it, 'e');
		++it;
		EXPECT_EQ(*it, 'f');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 0u);

	// Consume from the new buffer
	in << "ghi";
	{
		auto generator = channel.reader();
		auto maybeRange = co_await generator;
		EXPECT_EQ(channel.size(), 0u);
		auto it = maybeRange->begin();
		EXPECT_EQ(*it, 'g');
		++it;
		EXPECT_EQ(*it, 'h');
		++it;
		EXPECT_EQ(channel.size(), 0u);
	}
	EXPECT_EQ(channel.size(), 1u);

	co_return {};
}

TEST_ASYNC(IChannelBuffered, Read, (TestIChannel, TestIChannelZeroCopy, TestIChannelChunks, TestIChannelZeroCopyChunks))
{
	TestType in{};
	bzd::IChannelBuffered<char, 8u> channel{in};
	EXPECT_EQ(channel.size(), 0u);

	// Empty ichannel
	{
		auto maybeScope = co_await channel.read(12u);
		EXPECT_FALSE(maybeScope);
		EXPECT_EQ(maybeScope.error().getType(), bzd::ErrorType::eof);
		EXPECT_EQ(channel.size(), 0u);
	}

	in << "hello";
	{
		auto maybeScope = co_await channel.read(4u);
		EXPECT_TRUE(maybeScope);
		EXPECT_EQ(channel.size(), 0u);
	}

	{
		auto maybeScope = co_await channel.read(4u);
		EXPECT_TRUE(maybeScope);
		auto it = maybeScope->begin();
		EXPECT_EQ(*it, 'h');
		++it;
		EXPECT_EQ(*it, 'e');
		++it;
	}

	// Not enough samples
	{
		auto maybeScope = co_await channel.read(4u);
		EXPECT_FALSE(maybeScope);
		EXPECT_EQ(maybeScope.error().getType(), bzd::ErrorType::eof);
	}

	in << " me!";
	{
		auto maybeScope = co_await channel.read(4u);
		EXPECT_TRUE(maybeScope);
		auto it = maybeScope->begin();
		EXPECT_EQ(*it, 'l');
		++it;
		EXPECT_EQ(*it, 'l');
		++it;
		EXPECT_EQ(*it, 'o');
		++it;
		EXPECT_EQ(*it, ' ');
		++it;
	}

	// Just enough samples, need to fetch '!'
	{
		auto maybeScope = co_await channel.read(3u);
		EXPECT_TRUE(maybeScope);
		auto it = maybeScope->begin();
		EXPECT_EQ(*it, 'm');
		++it;
		EXPECT_EQ(*it, 'e');
		++it;
		EXPECT_EQ(*it, '!');
		++it;
	}

	co_return {};
}

using TestIChannelGenerator = bzd::test::IChannelGenerator<bzd::Int32, 32u>;
using TestIChannelGeneratorZeroCopy = bzd::test::IChannelGenerator<bzd::Int32, 32u, bzd::test::IChannelMode::zeroCopy>;
using TestIChannelGeneratorChunks = bzd::test::IChannelGenerator<bzd::Int32, 32u, bzd::test::IChannelMode::chunks>;
using TestIChannelGeneratorZeroCopyChunks =
	bzd::test::IChannelGenerator<bzd::Int32, 32u, bzd::test::IChannelMode::zeroCopy | bzd::test::IChannelMode::chunks>;

TEST_ASYNC(IChannelBuffered,
		   Stress,
		   (TestIChannelGenerator, TestIChannelGeneratorZeroCopy, TestIChannelGeneratorChunks, TestIChannelGeneratorZeroCopyChunks))
{
	TestType in{0};
	bzd::IChannelBuffered<bzd::Int32, 16u> channel{in};

	bzd::Int32 expected{0};
	for (bzd::Size iteration = 0u; iteration < 1000u || expected < 1000; ++iteration)
	{
		// Produce some data.
		const auto nbRead = test.random<bzd::Size, 1u, 16u>();
		auto maybeScope = co_await channel.read(nbRead);
		ASSERT_ASYNC_TRUE(maybeScope);

		// Read some of it.
		const auto nbMaxConsume = test.random<bzd::Size, 0u, 16u>();
		auto it = maybeScope->begin();
		for (bzd::Size i = 0u; i < nbMaxConsume && it != maybeScope->end(); ++i)
		{
			ASSERT_ASYNC_EQ(*it, expected);
			++it;
			++expected;
		}
	}

	co_return {};
}

TEST_ASYNC(IChannelBuffered,
		   BenchmarkReader,
		   (TestIChannelGenerator, TestIChannelGeneratorZeroCopy, TestIChannelGeneratorChunks, TestIChannelGeneratorZeroCopyChunks))
{
	TestType in{0};
	bzd::IChannelBuffered<bzd::Int32, 16u> channel{in};

	auto generator = channel.reader();

	for (bzd::Int32 expected = 0; expected < 1000;)
	{
		auto span = co_await generator;
		auto range = span.valueMutable();
		for (const auto value : range)
		{
			EXPECT_EQ(value, expected);
			++expected;
		}
	}

	co_return {};
}

TEST_ASYNC(IChannelBuffered,
		   BenchmarkReaderMove,
		   (TestIChannelGenerator, TestIChannelGeneratorZeroCopy, TestIChannelGeneratorChunks, TestIChannelGeneratorZeroCopyChunks))
{
	TestType in{0};
	bzd::IChannelBuffered<bzd::Int32, 16u> channel{in};

	auto generator = channel.reader();

	{
		auto result = co_await generator;
		auto it = bzd::begin(result.valueMutable());
		EXPECT_EQ(*it, 0);
		++it;
	}

	auto newGenerator = bzd::move(generator);	

	{
		auto result = co_await newGenerator;
		auto it = bzd::begin(result.valueMutable());
		EXPECT_EQ(*it, 1);
		++it;
	}

	co_return {};
}
