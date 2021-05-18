#include "bzd/container/vector.h"

#include "cc_test/test.h"

TEST(ContainerVector, Base)
{
	bzd::Vector<int, 5> test;
	EXPECT_EQ(test.capacity(), 5);
	EXPECT_EQ(test.size(), 0);
	test.pushBack(5);
	EXPECT_EQ(test.size(), 1);
	EXPECT_EQ(test[0], 5);
	test.pushBack(2);
	EXPECT_EQ(test.size(), 2);
	EXPECT_EQ(test[1], 2);

	bzd::interface::Vector<int>& iTest{test};
	EXPECT_EQ(iTest.size(), 2);
	EXPECT_EQ(iTest[1], 2);
	iTest.pushBack(-2);
	EXPECT_EQ(iTest.size(), 3);
	EXPECT_EQ(iTest[2], -2);
	EXPECT_EQ(test.size(), 3);
	EXPECT_EQ(test[2], -2);
}

TEST(ContainerVector, Constexpr)
{
	static constexpr bzd::Vector<const int, 5> test{1, 2, 5, 4, 6};
	constexpr const bzd::interface::Vector<const int>& iTest = test;

	EXPECT_EQ(iTest.capacity(), 5);
	EXPECT_EQ(iTest.size(), 5);

	static constexpr bzd::Vector<const int, 3> testDefault{};
	EXPECT_EQ(testDefault.capacity(), 3);
	EXPECT_EQ(testDefault.size(), 0);
}

TEST(ContainerVector, NonStaticConstexpr)
{
	constexpr bzd::VectorConstexpr<const int, 5> test{1, 2, 5, 4, 6};

	EXPECT_EQ(test.capacity(), 5);
	EXPECT_EQ(test.size(), 5);

	constexpr bzd::VectorConstexpr<const int, 3> testDefault{};
	EXPECT_EQ(testDefault.capacity(), 3);
	EXPECT_EQ(testDefault.size(), 0);
}