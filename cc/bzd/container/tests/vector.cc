#include "cc/bzd/container/vector.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/lifetime_counter.hh"
#include "cc/bzd/test/types/no_default_constructor.hh"

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

	bzd::interface::Vector<int>& iVector{vector};
	EXPECT_EQ(iVector.size(), 2U);
	EXPECT_EQ(iVector[1], 2);
	iVector.pushBack(-2);
	EXPECT_EQ(iVector.size(), 3U);
	EXPECT_EQ(iVector[2], -2);
	EXPECT_EQ(iVector.size(), 3U);
	EXPECT_EQ(iVector[2], -2);
}

TEST(ContainerVector, Lifetime)
{
	using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;

	bzd::Vector<LifetimeCounter, 5> vector;
	EXPECT_EQ(vector.capacity(), 5U);
	EXPECT_EQ(vector.size(), 0U);
	EXPECT_EQ(LifetimeCounter::constructor, 0U);
	EXPECT_EQ(LifetimeCounter::copy, 0U);
	EXPECT_EQ(LifetimeCounter::move, 0U);
	EXPECT_EQ(LifetimeCounter::destructor, 0U);
	vector.pushBack(LifetimeCounter{});
	EXPECT_EQ(LifetimeCounter::constructor, 1U);
	EXPECT_EQ(LifetimeCounter::copy, 0U);
	EXPECT_EQ(LifetimeCounter::move, 1U);
	EXPECT_EQ(LifetimeCounter::destructor, 1U);
	{
		LifetimeCounter copy{};
		vector[0] = copy;
		EXPECT_EQ(LifetimeCounter::constructor, 2U);
		EXPECT_EQ(LifetimeCounter::copy, 1U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
	}
	EXPECT_EQ(LifetimeCounter::destructor, 2U);
	vector.clear();
	EXPECT_EQ(LifetimeCounter::constructor, 2U);
	EXPECT_EQ(LifetimeCounter::copy, 1U);
	EXPECT_EQ(LifetimeCounter::move, 1U);
	EXPECT_EQ(LifetimeCounter::destructor, 3U);
}

TEST(ContainerVector, NoDefaultConstructor)
{
	bzd::Vector<bzd::test::NoDefaultConstructor, 5> vector;
	EXPECT_EQ(vector.capacity(), 5U);
	EXPECT_EQ(vector.size(), 0U);
	vector.pushBack(5);
	EXPECT_EQ(vector.size(), 1U);
	EXPECT_EQ(vector[0].data(), 5);
}

TEST(ContainerVector, Initializer)
{
	// initializer_list
	bzd::Vector<int, 5> vector1{1, 2, 3, 4};
	EXPECT_EQ(vector1.capacity(), 5U);
	EXPECT_EQ(vector1.size(), 4U);
	EXPECT_EQ(vector1[1], 2);

	// inplace
	bzd::Vector<int, 5> vector2{bzd::inPlace, 1, 2, 3, 4};
	EXPECT_EQ(vector2.capacity(), 5U);
	EXPECT_EQ(vector2.size(), 4U);
	EXPECT_EQ(vector2[1], 2);
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
