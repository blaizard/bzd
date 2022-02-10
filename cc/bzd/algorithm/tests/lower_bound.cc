#include "cc/bzd/algorithm/lower_bound.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(LowerBound, Base)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	{
		const auto it = bzd::algorithm::lowerBound(array.begin(), array.end(), 1);
		EXPECT_EQ(*it, 1);
	}

	{
		const auto it = bzd::algorithm::lowerBound(array, 0);
		EXPECT_EQ(*it, 0);
	}

	{
		const auto it = bzd::algorithm::lowerBound(array, 5);
		EXPECT_EQ(*it, 5);
	}

	{
		const auto it = bzd::algorithm::lowerBound(array, -2);
		EXPECT_EQ(*it, 0);
	}

	{
		const auto it = bzd::algorithm::lowerBound(array, 10);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::lowerBound(array, 4);
		EXPECT_EQ(*it, 5);
	}
}

TEST(LowerBound, EdgeCase)
{
	bzd::Array<int, 5> array{1, 2, 3, 4, 5};

	{
		const auto it = bzd::algorithm::lowerBound(array, 1);
		EXPECT_EQ(it, array.begin());
	}
}

TEST_CONSTEXPR_BEGIN(LowerBound, Constexpr)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	const auto it = bzd::algorithm::lowerBound(array.begin(), array.end(), 0);
	EXPECT_EQ(*it, 0);
}
TEST_CONSTEXPR_END(LowerBound, Constexpr)
