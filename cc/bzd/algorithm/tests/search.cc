#include "cc/bzd/algorithm/search.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Search, Base)
{
	bzd::Array<int, 5> array1{0, 1, 2, 3, 4};

	{
		bzd::Array<int, 4> array2{0, 1, 2, 3};
		const auto it = bzd::algorithm::search(array1, array2);
		EXPECT_EQ(it, array1.begin());
	}

	{
		bzd::Array<int, 3> array2{0, 1, 3};
		const auto it = bzd::algorithm::search(array1, array2);
		EXPECT_EQ(it, array1.end());
	}

	{
		bzd::Array<int, 2> array2{1, 2};
		const auto it = bzd::algorithm::search(array1, array2);
		const auto itExepcted = ++(array1.begin());
		EXPECT_EQ(it, itExepcted);
	}
}
