#include "gtest/gtest.h"
#include "include/container/vector.h"

TEST(ContainerVector, Base)
{
	bzd::Vector<int, 5> test;
	bzd::interface::Vector<int> iTest = test;

    EXPECT_EQ(iTest.capacity(), 5);
    EXPECT_EQ(iTest.size(), 0);
	iTest.pushBack(5);
    EXPECT_EQ(iTest.size(), 1);
    EXPECT_EQ(iTest[0], 5);
	iTest.pushBack(2);
    EXPECT_EQ(iTest.size(), 2);
    EXPECT_EQ(iTest[1], 2);
}
