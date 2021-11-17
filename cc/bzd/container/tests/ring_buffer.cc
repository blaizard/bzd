#include "cc/bzd/container/ring_buffer.hh"

#include "cc_test/test.hh"

TEST(RingBuffer, single)
{
	bzd::RingBuffer<int, 16> ring;

	EXPECT_EQ(ring.size(), 0);
	EXPECT_EQ(ring.capacity(), 16);
	EXPECT_TRUE(ring.empty());
	EXPECT_FALSE(ring.full());

	for (int i = 0; i < 15; ++i)
	{
		ring.pushBack(i);
	}

	EXPECT_EQ(ring.size(), 15);
	EXPECT_EQ(ring.capacity(), 16);
	EXPECT_FALSE(ring.empty());
	EXPECT_FALSE(ring.full());
	EXPECT_EQ(ring.at(0), 0);
	EXPECT_EQ(ring.at(1), 1);
	EXPECT_EQ(ring[14], 14);

	ring.pushBack(11);

	EXPECT_EQ(ring.size(), 16);
	EXPECT_EQ(ring.capacity(), 16);
	EXPECT_FALSE(ring.empty());
	EXPECT_TRUE(ring.full());
	EXPECT_FALSE(ring.overrun());

	ring.pushBack(12);

	EXPECT_EQ(ring.size(), 16);
	EXPECT_EQ(ring.capacity(), 16);
	EXPECT_FALSE(ring.empty());
	EXPECT_TRUE(ring.full());
	EXPECT_TRUE(ring.overrun());

	ring.clear();

	EXPECT_EQ(ring.size(), 0);
	EXPECT_EQ(ring.capacity(), 16);
	EXPECT_TRUE(ring.empty());
	EXPECT_FALSE(ring.full());
	EXPECT_FALSE(ring.overrun());
}

TEST(RingBuffer, asSpanForReading)
{
	bzd::RingBuffer<int, 16> ring;

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 0);
	}

	ring.pushBack(42);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 42);
	}

	ring.pushBack(34);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 2);
		EXPECT_EQ(span[0], 42);
		EXPECT_EQ(span[1], 34);
	}

	ring.consume(1);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 34);
	}

	for (int i = 0; i < 15; ++i)
	{
		ring.pushBack(i);
	}

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 15);
		EXPECT_EQ(span[0], 34);
		EXPECT_EQ(span[14], 13);
	}

	ring.consume(15);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 14);
	}

	ring.consume(1);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 0);
	}
}

TEST(RingBuffer, asSpanForWriting)
{
	bzd::RingBuffer<int, 16> ring;

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 16);
		int counter = 0;
		for (auto& i : span)
		{
			i = counter++;
		}
	}

	ring.produce(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 15);
		EXPECT_EQ(span[0], 1);
		EXPECT_EQ(span[14], 15);
	}

	ring.produce(14);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 15);
	}

	ring.produce(1);

	{
		EXPECT_TRUE(ring.full());
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 0);
	}

	ring.consume(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 0);
	}

	ring.consume(1);
	ring.produce(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 1);
	}

	ring.consume(1);
	ring.produce(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1);
		EXPECT_EQ(span[0], 2);
	}

	ring.consume(15);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 14);
		EXPECT_EQ(span[0], 2);
		EXPECT_EQ(span[13], 15);
	}
}
