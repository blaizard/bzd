#include "bzd/container/vector.h"
#include "cc_test/test.h"

TEST(ContainerVector, Base)
{
	bzd::Vector<int, 5> test;
	bzd::interface::Vector<int>& iTest = test;

	EXPECT_EQ(iTest.capacity(), 5);
	EXPECT_EQ(iTest.size(), 0);
	iTest.pushBack(5);
	EXPECT_EQ(iTest.size(), 1);
	EXPECT_EQ(iTest[0], 5);
	iTest.pushBack(2);
	EXPECT_EQ(iTest.size(), 2);
	EXPECT_EQ(iTest[1], 2);
}

TEST(ContainerVector, Constexpr)
{
	static constexpr bzd::Vector<const int, 5> test{1, 2, 5, 4, 6};
	constexpr const bzd::interface::Vector<const int>& iTest = test;

	EXPECT_EQ(iTest.capacity(), 5);
	EXPECT_EQ(iTest.size(), 5);
}
