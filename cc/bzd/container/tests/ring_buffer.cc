#include "cc/bzd/container/ring_buffer.hh"

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/min.hh"

TEST(RingBuffer, Simple)
{
	bzd::RingBuffer<int, 16> ring;

	EXPECT_EQ(ring.size(), 0U);
	EXPECT_EQ(ring.capacity(), 16U);
	EXPECT_TRUE(ring.empty());
	EXPECT_FALSE(ring.full());

	for (int i = 0; i < 15; ++i)
	{
		ring.pushBack(i);
	}

	EXPECT_EQ(ring.size(), 15U);
	EXPECT_EQ(ring.capacity(), 16U);
	EXPECT_FALSE(ring.empty());
	EXPECT_FALSE(ring.full());
	EXPECT_EQ(ring.at(0), 0);
	EXPECT_EQ(ring.at(1), 1);
	EXPECT_EQ(ring[14], 14);

	ring.pushBack(11);

	EXPECT_EQ(ring.size(), 16U);
	EXPECT_EQ(ring.capacity(), 16U);
	EXPECT_FALSE(ring.empty());
	EXPECT_TRUE(ring.full());
	EXPECT_FALSE(ring.overrun());

	ring.pushBack(12);

	EXPECT_EQ(ring.size(), 16U);
	EXPECT_EQ(ring.capacity(), 16U);
	EXPECT_FALSE(ring.empty());
	EXPECT_TRUE(ring.full());
	EXPECT_TRUE(ring.overrun());

	ring.clear();

	EXPECT_EQ(ring.size(), 0U);
	EXPECT_EQ(ring.capacity(), 16U);
	EXPECT_TRUE(ring.empty());
	EXPECT_FALSE(ring.full());
	EXPECT_FALSE(ring.overrun());
}

TEST(RingBuffer, AsSpanForReading)
{
	bzd::RingBuffer<int, 16> ring;

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 0U);
	}

	ring.pushBack(42);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 42);
	}

	ring.pushBack(34);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 2U);
		EXPECT_EQ(span[0], 42);
		EXPECT_EQ(span[1], 34);
	}

	ring.consume(1);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 34);
	}

	for (int i = 0; i < 15; ++i)
	{
		ring.pushBack(i);
	}

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 15U);
		EXPECT_EQ(span[0], 34);
		EXPECT_EQ(span[14], 13);
	}

	ring.consume(15);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 14);
	}

	ring.consume(1);

	{
		const auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 0U);
	}
}

TEST(RingBuffer, AsSpanForWriting)
{
	bzd::RingBuffer<int, 16> ring;

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 16U);
		int counter = 0;
		for (auto& i : span)
		{
			i = counter++;
		}
	}

	ring.produce(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 15U);
		EXPECT_EQ(span[0], 1);
		EXPECT_EQ(span[14], 15);
	}

	ring.produce(14);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 15);
	}

	ring.produce(1);

	{
		EXPECT_TRUE(ring.full());
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 0U);
	}

	ring.consume(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 0);
	}

	ring.consume(1);
	ring.produce(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 1);
	}

	ring.consume(1);
	ring.produce(1);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 1U);
		EXPECT_EQ(span[0], 2);
	}

	ring.consume(15);

	{
		auto span = ring.asSpanForWriting();
		EXPECT_EQ(span.size(), 14U);
		EXPECT_EQ(span[0], 2);
		EXPECT_EQ(span[13], 15);
	}
}

TEST(RingBuffer, AssignSpan)
{
	bzd::RingBuffer<int, 16> ring;

	{
		bzd::Array<int, 4> array{bzd::inPlace, 0, 1, 2, 3};
		ring.assign(array.asSpan());
		const auto isEqual = bzd::algorithm::equal(ring.asSpanForReading(), array);
		EXPECT_TRUE(isEqual);
		EXPECT_FALSE(ring.overrun());
	}
	{
		bzd::Array<int, 32> array{bzd::inPlace, 0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10, 11, 12, 13, 14, 15,
								  16,			17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
		ring.assign(array.asSpan());
		const auto isEqual = bzd::algorithm::equal(ring.asSpanForReading(), array.asSpan().subSpan(16, 16));
		EXPECT_TRUE(isEqual);
		EXPECT_TRUE(ring.overrun());
		EXPECT_TRUE(ring.full());
	}
	{
		ring.clear();
		ring.assign(ring.asSpanForWriting());
		auto span = ring.asSpanForReading();
		EXPECT_EQ(span.size(), 16u);
		EXPECT_FALSE(ring.overrun());
		EXPECT_TRUE(ring.full());
	}
}

TEST(RingBuffer, Unconsume)
{
	bzd::RingBuffer<int, 8> ring;
	const int a[] = {0, 1, 2, 3, 4, 5, 6, 7};
	const bzd::Span<const int> all{a};
	ring.assign(all);
	{
		EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(0, 1, 2, 3, 4, 5, 6, 7)));
	}
	ring.consume(4u);
	{
		EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(4, 5, 6, 7)));
	}
	EXPECT_EQ(ring.unconsume(all.first(4u)), 4u);
	{
		EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(0, 1, 2, 3, 4, 5, 6, 7)));
	}
	ring.produce(all.last(2u));
	EXPECT_TRUE(ring.overrun());
	{
		EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(2, 3, 4, 5, 6, 7, 6, 7)));
	}
	ring.consume(0u);
	{
		EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(2, 3, 4, 5, 6, 7, 6, 7)));
	}
}

TEST(RingBuffer, Produce)
{
	bzd::RingBuffer<int, 8> ring;
	ring.produce(bzd::makeArray(0, 1, 2, 3, 4, 5, 6).asSpan());
	EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(0, 1, 2, 3, 4, 5, 6)));
	ring.consume(3u);
	EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(3, 4, 5, 6)));
	ring.produce(bzd::makeArray(7, 8, 9).asSpan());
	EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(3, 4, 5, 6, 7, 8, 9)));
	EXPECT_FALSE(ring.overrun());
	ring.produce(bzd::makeArray(10, 11, 12).asSpan());
	EXPECT_TRUE(bzd::algorithm::equal(ring.asSpans(), bzd::makeArray(5, 6, 7, 8, 9, 10, 11, 12)));
	EXPECT_TRUE(ring.overrun());
}

TEST(RingBuffer, Stress)
{
	bzd::RingBuffer<int, 16> ring;
	int counter = 0;
	int counterRead = 0;

	for (int iteration = 0; iteration < 10000; ++iteration)
	{
		bzd::Size random = test.random<bzd::Size, 0, 16>();

		// Write X entries.
		if (test.random<bzd::Bool>())
		{
			auto span = ring.asSpanForWriting();
			const auto count = bzd::min(random, span.size());
			for (bzd::Size i = 0; i < count; ++i)
			{
				span[i] = counter++;
			}
			ring.produce(count);
		}
		// Read X entries.
		else
		{
			auto span = ring.asSpanForReading();
			const auto count = bzd::min(random, span.size());
			for (bzd::Size i = 0; i < count; ++i)
			{
				EXPECT_EQ(span[i], counterRead++);
			}
			ring.consume(count);
		}
	}
}
