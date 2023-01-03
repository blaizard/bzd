#include "cc/bzd/container/array.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerArray, Base)
{
	bzd::Array<int, 5> array;
	EXPECT_EQ(array.capacity(), 5U);
	EXPECT_EQ(array.size(), 5U);
	array[2] = 4;
	EXPECT_EQ(array[2], 4);
}

TEST(ContainerArray, Reference)
{
	int a = 1;
	bzd::Array<int&, 1> array{bzd::inPlace, a};
	EXPECT_EQ(array[0], 1);
	a = 2;
	EXPECT_EQ(array[0], 2);
}

TEST_CONSTEXPR_BEGIN(ContainerArray, Constexpr)
{
	constexpr bzd::Array<int, 5> array{};
	EXPECT_EQ(array.capacity(), 5U);
	EXPECT_EQ(array.size(), 5U);
}
TEST_CONSTEXPR_END(ContainerArray, Constexpr)
