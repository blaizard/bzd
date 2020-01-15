#include "bzd/container/tuple.h"

#include "gtest/gtest.h"

TEST(ContainerTuple, Base)
{
	bzd::Tuple<int, bool, double, int> tuple;

	// Accessor by index as template

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
	bzd::Tuple<unsigned int, bool, double, int, const char*> tuple(12, true, 5.32, -21, "Hello");

	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), true);
	EXPECT_NEAR(tuple.get<2>(), 5.32, 0.0001);
	EXPECT_EQ(tuple.get<3>(), -21);
	EXPECT_STREQ(tuple.get<4>(), "Hello");
}

TEST(ContainerTuple, ConstructorPartial)
{
	bzd::Tuple<unsigned int, bool, double, int> tuple(12, true);

	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), true);
}

TEST(ContainerTuple, Const)
{
	bzd::Tuple<const int, const bool, const double, const char*> tuple(12, false, 8.7, "Hello World");
	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), false);
}

TEST(ContainerTuple, Constexpr)
{
	constexpr const bzd::Tuple<int, const char*> tuple(12, "Hello");
	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_STREQ(tuple.get<1>(), "Hello");
}
