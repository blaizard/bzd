#include "bzd/container/array.h"

#include "cc_test/test.h"

TEST(ContainerArray, Base)
{
	bzd::Array<int, 5> test;
	EXPECT_EQ(test.capacity(), 5);
	EXPECT_EQ(test.size(), 5);
	test[2] = 4;
	EXPECT_EQ(test[2], 4);
}

TEST(ContainerArray, Constexpr)
{
	constexpr bzd::Array<int, 5> test{};
	EXPECT_EQ(test.capacity(), 5);
	EXPECT_EQ(test.size(), 5);
}