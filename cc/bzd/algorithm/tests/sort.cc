#include "cc/bzd/algorithm/sort.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/comparison/greater.hh"

TEST(Sort, Base)
{
	for (bzd::SizeType i = 0; i < 100; ++i)
	{
		bzd::Array<bzd::UInt32Type, 100> array;
		test.fillRandom(array);

		// Sort.
		bzd::algorithm::sort(array.begin(), array.end());

		// Verify.
		bzd::UInt32Type previous = 0;
		for (const auto& value : array)
		{
			EXPECT_GE(value, previous);
			previous = value;
		}
	}
}

TEST(Sort, CustomComparator)
{
	for (bzd::SizeType i = 0; i < 100; ++i)
	{
		bzd::Array<bzd::UInt32Type, 100> array;
		test.fillRandom(array);

		// Sort.
		bzd::algorithm::sort(array, bzd::Greater<bzd::UInt32Type>{});

		// Verify.
		bzd::UInt32Type previous = bzd::NumericLimits<bzd::UInt32Type>::max();
		for (const auto& value : array)
		{
			EXPECT_LE(value, previous);
			previous = value;
		}
	}
}

TEST_CONSTEXPR_BEGIN(Sort, Constexpr)
{
	bzd::Array<bzd::UInt32Type, 10> array{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	bzd::algorithm::sort(array);

	bzd::UInt32Type previous = 0;
	for (const auto& value : array)
	{
		EXPECT_GE(value, previous);
		previous = value;
	}
}
TEST_CONSTEXPR_END(Sort, Constexpr)
