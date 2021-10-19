#include "cc/bzd/meta/union.hh"

#include "cc/bzd/test/types.hh"
#include "cc/bzd/utility/move.hh"
#include "cc_test/test.hh"

#include <string>

TEST(MetaUnion, Constructor)
{
	bzd::meta::Union<int, bool, float> test;

	test.get<int>() = 21;
	EXPECT_EQ(test.get<int>(), 21);
	test.get<bool>() = true;
	EXPECT_TRUE(test.get<bool>());
	test.get<float>() = 3.435;
	EXPECT_NEAR(test.get<float>(), 3.435, 0.0001);

	// Constructor
	bzd::meta::Union<int, bool, float> testInt(static_cast<int>(42));
	EXPECT_EQ(testInt.get<int>(), 42);

	const bzd::meta::Union<int, bool, float> constTest(static_cast<bool>(true));
	EXPECT_TRUE(constTest.get<bool>());

	bzd::meta::Union<int, double> a{static_cast<double>(3.1415)};
	EXPECT_NEAR(a.get<double>(), 3.1415, 0.0001);

	bzd::meta::Union<char, void*> pointer{static_cast<void*>(nullptr)};
	EXPECT_EQ(pointer.get<void*>(), nullptr);
}

TEST(MetaUnionConstexpr, Constructor)
{
	// Trivial types can be constexpr
	constexpr bzd::meta::Union<int, bool, float> constexprTest(static_cast<float>(32.5));
	EXPECT_NEAR(constexprTest.get<float>(), 32.5, 0.001);

	// Constexpr Copy Constructor
	constexpr auto constexprTest2(constexprTest);
	constexpr auto constexprTest3 = constexprTest2;
	EXPECT_NEAR(constexprTest3.get<float>(), 32.5, 0.001);

	// Complex types, it cannot be constexpr
	bzd::meta::Union<bool, std::string> testComplex(std::string("Hello"));
	EXPECT_STREQ(testComplex.get<std::string>().c_str(), "Hello");
}

TEST(MetaUnion, Empty)
{
	using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
	{
		bzd::meta::Union<LifetimeCounter> u{};
		EXPECT_EQ(LifetimeCounter::constructor_, 0);
		EXPECT_EQ(LifetimeCounter::copy_, 0);
		EXPECT_EQ(LifetimeCounter::move_, 0);
		EXPECT_EQ(LifetimeCounter::destructor_, 0);
	}
	EXPECT_EQ(LifetimeCounter::destructor_, 0);
}

TEST(MetaUnion, CopyConstructor)
{
	using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
	{
		LifetimeCounter value{};
		EXPECT_EQ(LifetimeCounter::constructor_, 1);
		EXPECT_EQ(LifetimeCounter::copy_, 0);
		EXPECT_EQ(LifetimeCounter::move_, 0);
		EXPECT_EQ(LifetimeCounter::destructor_, 0);
		bzd::meta::Union<LifetimeCounter> u{value};
		EXPECT_EQ(LifetimeCounter::constructor_, 1);
		EXPECT_EQ(LifetimeCounter::copy_, 1);
		EXPECT_EQ(LifetimeCounter::move_, 0);
		EXPECT_EQ(LifetimeCounter::destructor_, 0);
	}
	EXPECT_EQ(LifetimeCounter::destructor_, 1);

	{
		bzd::test::CopyOnly value{};
		bzd::meta::Union<bzd::test::CopyOnly> u{value};
		EXPECT_EQ(u.get<bzd::test::CopyOnly>().getCopiedCounter(), 1);
		bzd::meta::Union<bzd::test::CopyOnly> u2;
		u2.get<bzd::test::CopyOnly>() = u.get<bzd::test::CopyOnly>();
		EXPECT_EQ(u2.get<bzd::test::CopyOnly>().getCopiedCounter(), 2);
	}
}

TEST(MetaUnion, MoveConstructor)
{
	using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
	{
		LifetimeCounter value{};
		EXPECT_EQ(LifetimeCounter::constructor_, 1);
		EXPECT_EQ(LifetimeCounter::copy_, 0);
		EXPECT_EQ(LifetimeCounter::move_, 0);
		EXPECT_EQ(LifetimeCounter::destructor_, 0);
		bzd::meta::Union<LifetimeCounter> u{bzd::move(value)};
		EXPECT_EQ(LifetimeCounter::constructor_, 1);
		EXPECT_EQ(LifetimeCounter::copy_, 0);
		EXPECT_EQ(LifetimeCounter::move_, 1);
		EXPECT_EQ(LifetimeCounter::destructor_, 0);
	}
	EXPECT_EQ(LifetimeCounter::destructor_, 1);

	{
		bzd::test::MoveOnly value{};
		bzd::meta::Union<bzd::test::MoveOnly> u{bzd::move(value)};
		EXPECT_EQ(u.get<bzd::test::MoveOnly>().getMovedCounter(), 1);
		bzd::meta::Union<bzd::test::MoveOnly> u2;
		u2.get<bzd::test::MoveOnly>() = bzd::move(u.get<bzd::test::MoveOnly>());
		EXPECT_EQ(u2.get<bzd::test::MoveOnly>().getMovedCounter(), 2);
		bzd::meta::Union<bzd::test::MoveOnly> u3{bzd::test::MoveOnly{}};
		EXPECT_EQ(u3.get<bzd::test::MoveOnly>().getMovedCounter(), 1);
	}
}
