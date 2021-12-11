#include "cc/bzd/algorithm/equal.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Equal, Base)
{
	bzd::Array<int, 5> array1{0, 1, 2, 3, 4};
	bzd::Array<int, 4> array2{0, 1, 2, 3};
	bzd::Array<int, 5> array3{0, 1, 2, 3, 5};

	{
		const auto isEqual = bzd::algorithm::equal(array2.begin(), array2.end(), array1.begin());
		EXPECT_TRUE(isEqual);
	}

	{
		const auto isEqual = bzd::algorithm::equal(array1.begin(), array1.end(), array1.begin());
		EXPECT_TRUE(isEqual);
	}

	{
		const auto isEqual = bzd::algorithm::equal(array1.begin(), array1.begin(), array2.begin());
		EXPECT_TRUE(isEqual);
	}

	{
		const auto isEqual = bzd::algorithm::equal(array1.begin(), array1.end(), array3.begin());
		EXPECT_FALSE(isEqual);
	}
}

TEST_CONSTEXPR_BEGIN(Equal, Constexpr)
{
	bzd::Array<int, 4> array{0, 1, 2, 3};

	const auto isEqual = bzd::algorithm::equal(array.begin(), array.end(), array.begin());
	EXPECT_TRUE(isEqual);
}
TEST_CONSTEXPR_END(Equal, Constexpr)
