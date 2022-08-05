#include "cc/bzd/algorithm/rsearch.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Search, Base)
{
	bzd::Array<int, 7> array1{0, 1, 2, 3, 4, 1, 2};

	{
		bzd::Array<int, 4> array2{0, 1, 2, 3};
		const auto it = bzd::algorithm::rsearch(array1, array2);
		EXPECT_EQ(it, array1.begin());
	}

	{
		bzd::Array<int, 3> array2{0, 1, 3};
		const auto it = bzd::algorithm::rsearch(array1, array2);
		EXPECT_EQ(it, array1.end());
	}

	{
		bzd::Array<int, 2> array2{1, 2};
		const auto it = bzd::algorithm::rsearch(array1, array2);
		const auto itExpected = (array1.begin()) + 5;
		EXPECT_EQ(it, itExpected);
	}
}
