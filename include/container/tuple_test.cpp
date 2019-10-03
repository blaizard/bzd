#include "gtest/gtest.h"
#include "include/container/tuple.h"

TEST(ContainerTuple, Base)
{
	bzd::Tuple<int, bool, double> tuple;

	tuple.get<0>() = 12;
	tuple.get<1>() = true;
	tuple.get<2>() = 2.4;

	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), true);
	EXPECT_NEAR(tuple.get<2>(), 2.4, 0.0001);
}
