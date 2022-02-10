#include "cc/bzd/algorithm/binary_search.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(BinarySearch, Base)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	{
		const auto it = bzd::algorithm::binarySearch(array.begin(), array.end(), 1);
		EXPECT_EQ(*it, 1);
	}

	{
		const auto it = bzd::algorithm::binarySearch(array, 0);
		EXPECT_EQ(*it, 0);
	}

	{
		const auto it = bzd::algorithm::binarySearch(array, 4);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::binarySearch(array, 5);
		EXPECT_EQ(*it, 5);
	}

	{
		const auto it = bzd::algorithm::binarySearch(array, -23);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::binarySearch(array, 35);
		EXPECT_EQ(it, array.end());
	}
}

TEST_CONSTEXPR_BEGIN(BinarySearch, Constexpr)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	const auto it = bzd::algorithm::binarySearch(array.begin(), array.end(), 0);
	EXPECT_EQ(*it, 0);
}
TEST_CONSTEXPR_END(BinarySearch, Constexpr)
