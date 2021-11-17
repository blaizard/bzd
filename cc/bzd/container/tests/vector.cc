#include "cc/bzd/container/vector.hh"

#include "cc_test/test.hh"

TEST(ContainerVector, Base)
{
	bzd::Vector<int, 5> vector;
	EXPECT_EQ(vector.capacity(), 5);
	EXPECT_EQ(vector.size(), 0);
	vector.pushBack(5);
	EXPECT_EQ(vector.size(), 1);
	EXPECT_EQ(vector[0], 5);
	vector.pushBack(2);
	EXPECT_EQ(vector.size(), 2);
	EXPECT_EQ(vector[1], 2);

	bzd::interface::Vector<int>& iVector{vector};
	EXPECT_EQ(iVector.size(), 2);
	EXPECT_EQ(iVector[1], 2);
	iVector.pushBack(-2);
	EXPECT_EQ(iVector.size(), 3);
	EXPECT_EQ(iVector[2], -2);
	EXPECT_EQ(iVector.size(), 3);
	EXPECT_EQ(iVector[2], -2);
}

TEST(ContainerVector, Constexpr)
{
	static constexpr bzd::Vector<const int, 5> vector{1, 2, 5, 4, 6};
	constexpr const bzd::interface::Vector<const int>& iVector = vector;

	EXPECT_EQ(iVector.capacity(), 5);
	EXPECT_EQ(iVector.size(), 5);

	static constexpr bzd::Vector<const int, 3> testDefault{};
	EXPECT_EQ(testDefault.capacity(), 3);
	EXPECT_EQ(testDefault.size(), 0);
}

TEST(ContainerVector, NonStaticConstexpr)
{
	constexpr bzd::VectorConstexpr<const int, 5> vector{1, 2, 5, 4, 6};

	EXPECT_EQ(vector.capacity(), 5);
	EXPECT_EQ(vector.size(), 5);

	constexpr bzd::VectorConstexpr<const int, 3> testDefault{};
	EXPECT_EQ(testDefault.capacity(), 3);
	EXPECT_EQ(testDefault.size(), 0);
}
