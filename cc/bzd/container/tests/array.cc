#include "cc/bzd/container/array.hh"

#include "cc_test/test.hh"

TEST(ContainerArray, Base)
{
	bzd::Array<int, 5> array;
	EXPECT_EQ(array.capacity(), 5);
	EXPECT_EQ(array.size(), 5);
	array[2] = 4;
	EXPECT_EQ(array[2], 4);
}

TEST(ContainerArray, Constexpr)
{
	constexpr bzd::Array<int, 5> array{};
	EXPECT_EQ(array.capacity(), 5);
	EXPECT_EQ(array.size(), 5);
}
