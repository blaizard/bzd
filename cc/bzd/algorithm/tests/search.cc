#include "cc/bzd/algorithm/search.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Search, Base)
{
	bzd::Array<int, 5> array1{0, 1, 2, 3, 2};

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

	{
		bzd::Array<int, 1> array2{2};
		const auto it = bzd::algorithm::search(array1, array2);
		const auto itExepcted = array1.begin() + 2;
		EXPECT_EQ(it, itExepcted);
	}

	{
		const auto it = bzd::algorithm::rsearch(array1.begin(), array1.end(), array1.begin(), array1.begin());
		EXPECT_EQ(it, array1.end());
	}

	{
		const auto it = bzd::algorithm::rsearch(array1.begin(), array1.begin(), array1.begin(), array1.end());
		EXPECT_EQ(it, array1.begin());
	}
}
