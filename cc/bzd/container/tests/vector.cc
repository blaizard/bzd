#include "cc/bzd/container/vector.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerVector, Base)
{
	bzd::Vector<int, 5> vector;
	EXPECT_EQ(vector.capacity(), 5U);
	EXPECT_EQ(vector.size(), 0U);
	vector.pushBack(5);
	EXPECT_EQ(vector.size(), 1U);
	EXPECT_EQ(vector[0], 5);
	vector.pushBack(2);
	EXPECT_EQ(vector.size(), 2U);
	EXPECT_EQ(vector[1], 2);

	bzd::interface::Vector<int>& iVector {
		vector
	};
	EXPECT_EQ(iVector.size(), 2U);
	EXPECT_EQ(iVector[1], 2);
	iVector.pushBack(-2);
	EXPECT_EQ(iVector.size(), 3U);
	EXPECT_EQ(iVector[2], -2);
	EXPECT_EQ(iVector.size(), 3U);
	EXPECT_EQ(iVector[2], -2);
}

TEST_CONSTEXPR_BEGIN(ContainerVector, Constexpr)
{
	bzd::Vector<const int, 5> vector{bzd::inPlace, 1, 2, 5, 4, 6};
	const bzd::interface::Vector<const int>& iVector = vector;

	EXPECT_EQ(iVector.capacity(), 5U);
	EXPECT_EQ(iVector.size(), 5U);

	bzd::Vector<const int, 3> testDefault{};
	EXPECT_EQ(testDefault.capacity(), 3U);
	EXPECT_EQ(testDefault.size(), 0U);
}
TEST_CONSTEXPR_END(ContainerVector, Constexpr)

TEST(ContainerVector, NonStaticConstexpr)
{
	constexpr bzd::VectorConstexpr<const int, 5> vector{bzd::inPlace, 1, 2, 5, 4, 6};

	EXPECT_EQ(vector.capacity(), 5U);
	EXPECT_EQ(vector.size(), 5U);

	constexpr bzd::VectorConstexpr<const int, 3> testDefault{};
	EXPECT_EQ(testDefault.capacity(), 3U);
	EXPECT_EQ(testDefault.size(), 0U);
}
