#include "cc/bzd/algorithm/upper_bound.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(UpperBound, Base)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 1);
		EXPECT_EQ(*it, 2);
	}

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 0);
		EXPECT_EQ(*it, 1);
	}

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 5);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), -2);
		EXPECT_EQ(*it, 0);
	}

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 10);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 4);
		EXPECT_EQ(*it, 5);
	}
}

TEST(UpperBound, EdgeCase)
{
	bzd::Array<int, 5> array{1, 2, 3, 4, 5};

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.begin(), 1);
		EXPECT_EQ(it, array.begin());
	}
}

TEST_CONSTEXPR_BEGIN(UpperBound, Constexpr)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

	const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 0);
	EXPECT_EQ(*it, 1);
}
TEST_CONSTEXPR_END(UpperBound, Constexpr)
