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
		auto scope = container.asSpansForReading();
		ASSERT_FALSE(scope);
	}

	// Write something.
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 0u);
		scope.valueMutable() = 12;
	}
	ASSERT_EQ(container.size(), 1u);

	// Try to read again with firstForReading.
	{
		auto scope = container.firstForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 0u);
		ASSERT_EQ(scope.value(), 12);
	}

	// Try to read again with firstForReading with start.
	{
		auto scope = container.firstForReading(/*start*/ 1u);
		ASSERT_FALSE(scope);
	}

	// Try to read again with lastForReading.
	{
		auto scope = container.lastForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 0u);
		ASSERT_EQ(scope.value(), 12);
	}

	// Try to read again with lastForReading with start.
	{
		auto scope = container.lastForReading(/*start*/ 1u);
		ASSERT_FALSE(scope);
	}

	// Write 2 entries.
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 1u);
		scope.valueMutable() = 13;
	}
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 2u);
		scope.valueMutable() = 14;
	}
	ASSERT_EQ(container.size(), 3u);

	// Try to read again with firstForReading.
	{
		auto scope = container.firstForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 0u);
		ASSERT_EQ(scope.value(), 12);
	}

	// Try to read again with firstForReading with start.
	{
		auto scope = container.firstForReading(/*start*/ 1u);
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 1u);
		ASSERT_EQ(scope.value(), 13);
	}

	// Try to read again with lastForReading.
	{
		auto scope = container.lastForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 2u);
		ASSERT_EQ(scope.value(), 14);
	}

	// Try to read again with lastForReading with start.
	{
		auto scope = container.lastForReading(/*start*/ 2u);
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 2u);
		ASSERT_EQ(scope.value(), 14);
	}

	// Read all the entries available for reading.
	{
		auto scope = container.asSpansForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 0u);
		ASSERT_EQ(scope.value().size(), 3u);
		{
			const auto expected = {12, 13, 14};
			const auto isEqual = bzd::algorithm::equal(scope.value(), expected);
			EXPECT_TRUE(isEqual);
		}
	}

	// Read certain entries only.
	{
		auto scope = container.asSpansForReading(/*count*/ 1, /*first*/ true);
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 0u);
		ASSERT_EQ(scope.value().size(), 1u);
		{
			const auto expected = {12};
			const auto isEqual = bzd::algorithm::equal(scope.value(), expected);
			EXPECT_TRUE(isEqual);
		}
	}
	{
		auto scope = container.asSpansForReading(/*count*/ 1, /*first*/ false);
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 2u);
		ASSERT_EQ(scope.value().size(), 1u);
		{
			const auto expected = {14};
			const auto isEqual = bzd::algorithm::equal(scope.value(), expected);
			EXPECT_TRUE(isEqual);
		}
	}
	{
		auto scope = container.asSpansForReading(/*count*/ 2, /*first*/ false);
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 1u);
		ASSERT_EQ(scope.value().size(), 2u);
		{
			const auto expected = {13, 14};
			const auto isEqual = bzd::algorithm::equal(scope.value(), expected);
			EXPECT_TRUE(isEqual);
		}
	}

	for (bzd::Int8 i = 15; i < 30; ++i)
	{
		auto scope = container.nextForWriting();
		ASSERT_TRUE(scope);
		scope.valueMutable() = i;
	}
	ASSERT_EQ(container.size(), 10u);

	// Read all the entries available for reading.
	{
		auto scope = container.asSpansForReading();
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 8u);
		ASSERT_EQ(scope.value().size(), 10u);
		{
			const auto expected = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
			const auto isEqual = bzd::algorithm::equal(scope.value(), expected);
			EXPECT_TRUE(isEqual);
		}
	}

	// Read all the entries available for reading starting at a specific index.
	{
		auto scope = container.asSpansForReading(/*count*/ 0, /*first*/ true, /*start*/ 12u);
		ASSERT_TRUE(scope);
		ASSERT_EQ(scope.index(), 12u);
		ASSERT_EQ(scope.value().size(), 6u);
		{
			const auto expected = {24, 25, 26, 27, 28, 29};
			const auto isEqual = bzd::algorithm::equal(scope.value(), expected);
			EXPECT_TRUE(isEqual);
		}
	}
}
