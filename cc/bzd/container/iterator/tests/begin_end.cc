#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/iterator/begin.hh"
#include "cc/bzd/container/iterator/end.hh"
#include "cc/bzd/test/test.hh"

TEST(IteratorBegin, array)
{
	int temp[10]{};
	auto it = bzd::begin(temp);
	EXPECT_EQ(it, temp);
}

TEST(IteratorBegin, container)
{
	bzd::Array<int, 10> temp{};
	auto it = bzd::begin(temp);
	EXPECT_EQ(it, temp.begin());
}

TEST(IteratorEnd, array)
{
	int temp[10]{};
	auto it = bzd::end(temp);
	EXPECT_EQ(it, temp + 10);
}

TEST(IteratorEnd, container)
{
	bzd::Array<int, 10> temp{};
	auto it = bzd::end(temp);
	EXPECT_EQ(it, temp.end());
}
