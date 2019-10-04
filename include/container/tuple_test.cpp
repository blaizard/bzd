#include "gtest/gtest.h"
#include "include/container/tuple.h"

TEST(ContainerTuple, Base)
{
	bzd::Tuple<int, bool, double, int> tuple;

	tuple.get<0>() = 12;
	tuple.get<1>() = true;
	tuple.get<2>() = 2.4;
	tuple.get<3>() = -32;

	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), true);
	EXPECT_NEAR(tuple.get<2>(), 2.4, 0.0001);
	EXPECT_EQ(tuple.get<3>(), -32);
}

TEST(ContainerTuple, Constructor)
{
	bzd::Tuple<unsigned int, bool, double, int> tuple(12, true, 5.32, -21);

	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), true);
	EXPECT_NEAR(tuple.get<2>(), 5.32, 0.0001);
	EXPECT_EQ(tuple.get<3>(), -21);
}
