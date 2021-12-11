#include "cc/bzd/algorithm/lexicographical_compare.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(LexicographicalCompare, Base)
{
	bzd::Array<int, 5> array1{0, 1, 2, 3, 4};
	bzd::Array<int, 5> array2{1, 2, 3, 4, 5};
	bzd::Array<int, 5> array3{0, 1, 2, 3, 6};

	{
		const auto isLower = bzd::algorithm::lexicographicalCompare(array1.begin(), array1.end(), array2.begin(), array2.end());
		EXPECT_TRUE(isLower);
		const auto isInverseLower = bzd::algorithm::lexicographicalCompare(array2.begin(), array2.end(), array1.begin(), array1.end());
		EXPECT_FALSE(isInverseLower);
	}

	{
		const auto isLower = bzd::algorithm::lexicographicalCompare(array1.begin(), array1.end(), array3.begin(), array3.end());
		EXPECT_TRUE(isLower);
		const auto isInverseLower = bzd::algorithm::lexicographicalCompare(array3.begin(), array3.end(), array1.begin(), array1.end());
		EXPECT_FALSE(isInverseLower);
	}

	{
		const auto isLower = bzd::algorithm::lexicographicalCompare(array1.begin(), array1.end(), array1.begin(), array1.end());
		EXPECT_FALSE(isLower);
		const auto isInverseLower = bzd::algorithm::lexicographicalCompare(array1.begin(), array1.end(), array1.begin(), array1.end());
		EXPECT_FALSE(isInverseLower);
	}

	{
		const auto isLower = bzd::algorithm::lexicographicalCompare(array2.begin(), array2.end(), array3.begin(), array3.end());
		EXPECT_FALSE(isLower);
		const auto isInverseLower = bzd::algorithm::lexicographicalCompare(array3.begin(), array3.end(), array2.begin(), array2.end());
		EXPECT_TRUE(isInverseLower);
	}
}

TEST_CONSTEXPR_BEGIN(LexicographicalCompare, Constexpr)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	const auto isLower = bzd::algorithm::lexicographicalCompare(array.begin(), array.end(), array.begin(), array.end());
	EXPECT_FALSE(isLower);
}
TEST_CONSTEXPR_END(LexicographicalCompare, Constexpr)
