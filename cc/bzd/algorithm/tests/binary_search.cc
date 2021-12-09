#include "cc/bzd/algorithm/binary_search.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(BinarySearch, Base)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

    {
        const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), 1);
        EXPECT_TRUE(result);
    }

    {
        const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), 0);
        EXPECT_TRUE(result);
    }

    {
        const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), 4);
        EXPECT_FALSE(result);
    }

    {
        const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), 5);
        EXPECT_TRUE(result);
    }

    {
        const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), -23);
        EXPECT_FALSE(result);
    }

    {
        const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), 35);
        EXPECT_FALSE(result);
    }
}

TEST_CONSTEXPR_BEGIN(BinarySearch, Constexpr)
{
	bzd::Array<int, 5> array{0, 1, 2, 3, 5};

    const auto result = bzd::algorithm::binarySearch(array.begin(), array.end(), 0);
    EXPECT_TRUE(result);
}
TEST_CONSTEXPR_END(BinarySearch, Constexpr)
