#include "cc/bzd/algorithm/sort.hh"

#include "cc/bzd/container/array.hh"
#include "cc_test/test.hh"

TEST(Sort, Base)
{
	for (bzd::SizeType i = 0; i < 100; ++i)
	{
		bzd::Array<bzd::UInt32Type, 100> array;

		// Fill with random values.
		for (auto& value : array)
		{
			value = test.randInt<bzd::UInt32Type>(0, 1000);
		}

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

TEST_CONSTEXPR_BEGIN(Sort, Constexpr)
{
	bzd::Array<bzd::UInt32Type, 10> array{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	bzd::algorithm::sort(array.begin(), array.end());

	bzd::UInt32Type previous = 0;
	for (const auto& value : array)
	{
		EXPECT_GE(value, previous);
		previous = value;
	}
}
TEST_CONSTEXPR_END(Sort, Constexpr)
