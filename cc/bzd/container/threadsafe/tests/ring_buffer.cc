#include "cc/bzd/container/threadsafe/ring_buffer.hh"

#include "cc/bzd/test/test.hh"

TEST(RingBuffer, Basic)
{
	bzd::threadsafe::RingBuffer<bzd::Int8, 10> container;
	ASSERT_EQ(container.size(), 0u);

	// Try to read with firstForReading.
	{
		auto scope = container.firstForReading();
		ASSERT_FALSE(scope);
	}

	// Try to read with lastForReading.
	{
		auto scope = container.lastForReading();
		ASSERT_FALSE(scope);
	}

	// Try to read with asSpansForReading.
	{
		auto range = container.asSpansForReading();
		ASSERT_EQ(range.size(), 0u);
		ASSERT_TRUE(range.empty());
	}

	// Write something.
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		scope.valueMutable() = 12;
	}
	ASSERT_EQ(container.size(), 1u);

	// Try to read again with firstForReading.
	{
		auto scope = container.firstForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.value(), 12);
	}

	// Try to read again with lastForReading.
	{
		auto scope = container.lastForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.value(), 12);
	}

	// Write 2 entries.
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		scope.valueMutable() = 13;
	}
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		scope.valueMutable() = 14;
	}
	ASSERT_EQ(container.size(), 3u);

	// Try to read again with firstForReading.
	{
		auto scope = container.firstForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.value(), 12);
	}

	// Try to read again with lastForReading.
	{
		auto scope = container.lastForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.value(), 14);
	}

	// Read all the entries available for reading.
	{
		auto range = container.asSpansForReading();
		ASSERT_EQ(range.size(), 3u);
		{
			const auto expected = {12, 13, 14};
			const auto isEqual = bzd::algorithm::equal(range, expected);
			EXPECT_TRUE(isEqual);
		}
	}

	// Read certain entries only.
	{
		auto range = container.asSpansForReading(/*count*/ 1, /*first*/ true);
		ASSERT_EQ(range.size(), 1u);
		{
			const auto expected = {12};
			const auto isEqual = bzd::algorithm::equal(range, expected);
			EXPECT_TRUE(isEqual);
		}
	}
	{
		auto range = container.asSpansForReading(/*count*/ 1, /*first*/ false);
		ASSERT_EQ(range.size(), 1u);
		{
			const auto expected = {14};
			const auto isEqual = bzd::algorithm::equal(range, expected);
			EXPECT_TRUE(isEqual);
		}
	}
	{
		auto range = container.asSpansForReading(/*count*/ 2, /*first*/ false);
		ASSERT_EQ(range.size(), 2u);
		{
			const auto expected = {13, 14};
			const auto isEqual = bzd::algorithm::equal(range, expected);
			EXPECT_TRUE(isEqual);
		}
	}

    for (bzd::Int8 i = 15; i<30; ++i)
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		scope.valueMutable() = i;
	}
	ASSERT_EQ(container.size(), 10u);

	// Read all the entries available for reading.
	{
		auto range = container.asSpansForReading();
		ASSERT_EQ(range.size(), 10u);
		{
			const auto expected = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
			const auto isEqual = bzd::algorithm::equal(range, expected);
			EXPECT_TRUE(isEqual);
		}
	}
}
