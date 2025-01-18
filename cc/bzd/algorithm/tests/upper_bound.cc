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
		const auto it = bzd::algorithm::upperBound(array, 0);
		EXPECT_EQ(*it, 1);
	}

	{
		const auto it = bzd::algorithm::upperBound(array, 5);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::upperBound(array, -2);
		EXPECT_EQ(*it, 0);
	}

	{
		const auto it = bzd::algorithm::upperBound(array, 10);
		EXPECT_EQ(it, array.end());
	}

	{
		const auto it = bzd::algorithm::upperBound(array, 4);
		EXPECT_EQ(*it, 5);
	}
}

TEST(UpperBound, StringView)
{
	bzd::Array<bzd::StringView, 5> array{"abc"_sv, "de"_sv, "def"_sv, "pq"_sv, "z"_sv};

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), "c"_sv);
		EXPECT_EQ(*it, "de"_sv);
	}

	{
		const auto it = bzd::algorithm::upperBound(array, "a"_sv);
		EXPECT_EQ(*it, "abc"_sv);
	}

	{
		const auto it = bzd::algorithm::upperBound(array, "de"_sv);
		EXPECT_EQ(*it, "def"_sv);
	}

	{
		const auto it = bzd::algorithm::upperBound(array, "pqr"_sv);
		EXPECT_EQ(*it, "z"_sv);
	}

	{
		const auto it = bzd::algorithm::upperBound(array, "z"_sv);
		EXPECT_EQ(it, array.end());
	}
}

TEST(UpperBound, EdgeCase)
{
	bzd::Array<int, 5> array{1, 2, 3, 4, 5};

	{
		const auto it = bzd::algorithm::upperBound(array.begin(), array.end(), 0);
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
