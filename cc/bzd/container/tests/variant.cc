#include "cc/bzd/container/variant.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/copy_only.hh"
#include "cc/bzd/test/types/lifetime_counter.hh"
#include "cc/bzd/test/types/move_only.hh"
#include "cc/bzd/test/types/no_default_constructor.hh"

TEST(ContainerVariant, Constructor)
{
	bzd::Variant<int, bool, double> variant;
	EXPECT_EQ(variant.index(), 0);
	bzd::Variant<int, bool, double> variantInt{static_cast<int>(45)};
	EXPECT_EQ(variantInt.index(), 0);
	bzd::Variant<int, bool, double> variantBool{static_cast<bool>(true)};
	EXPECT_EQ(variantBool.index(), 1);
	bzd::Variant<int, bool, double> variantDouble{static_cast<double>(5.4)};
	EXPECT_EQ(variantDouble.index(), 2);
	bzd::Variant<bzd::test::NoDefaultConstructor, bool, double> variantNoDefault{static_cast<double>(5.4)};
	EXPECT_EQ(variantNoDefault.index(), 2)

	// In place constructors
	{
		bzd::Variant<int, bool, double> variantInPlaceIndex1{bzd::inPlaceIndex<2>, 3.2};
		EXPECT_EQ(variantInPlaceIndex1.index(), 2);
		bzd::Variant<int, bool, double> variantInPlaceType1{bzd::inPlaceType<int>, 3};
		EXPECT_EQ(variantInPlaceType1.index(), 0);
	}
}

TEST(ContainerVariant, Reference)
{
	{
		double value{12};
		bzd::Variant<int&, bool&, double&> variantReference{bzd::inPlaceIndex<2>, value};
		EXPECT_EQ(variantReference.index(), 2);
		EXPECT_EQ(variantReference.template get<double&>(), 12);
		value = 3;
		EXPECT_EQ(value, 3);
		EXPECT_EQ(variantReference.template get<double&>(), 3);
		int valueInt{65};
		variantReference.template set<int&>(valueInt);
		EXPECT_EQ(variantReference.index(), 0);
		EXPECT_EQ(variantReference.template get<int&>(), 65);
		valueInt = -3;
		EXPECT_EQ(valueInt, -3);
		EXPECT_EQ(variantReference.template get<int&>(), -3);
		variantReference.match([](int& v) { v = 5; }, [](bool&) {}, [](double&) {});
		EXPECT_EQ(variantReference.template get<int&>(), 5);

		bzd::Variant<int&, bool&, double&> variantReferenceMove{bzd::move(variantReference)};
		EXPECT_EQ(variantReferenceMove.template get<int&>(), 5);
		valueInt = -3;
		EXPECT_EQ(valueInt, -3);
		EXPECT_EQ(variantReferenceMove.template get<int&>(), -3);
	}
	{
		int value{12};
		bzd::Variant<int&, bool&, double&> variantReference{bzd::inPlaceType<int&>, value};
		EXPECT_EQ(variantReference.index(), 0);
		EXPECT_EQ(variantReference.template get<int&>(), 12);
		value = 3;
		EXPECT_EQ(value, 3);
		EXPECT_EQ(variantReference.template get<int&>(), 3);
	}
}

TEST(ContainerVariant, Comparators)
{
	const bzd::Variant<int, bool, double> variantInt1{static_cast<int>(45)};
	const bzd::Variant<int, bool, double> variantInt2{static_cast<int>(12)};
	EXPECT_NE(variantInt1, variantInt2);
	const bzd::Variant<int, bool, double> variantInt3{static_cast<int>(45)};
	EXPECT_EQ(variantInt1, variantInt3);
	const bzd::Variant<int, bool, double> variantInt4{static_cast<double>(4.5)};
	EXPECT_NE(variantInt1, variantInt4);
	const bzd::Variant<int, bool, double> variantInt5{static_cast<double>(4.5)};
	EXPECT_EQ(variantInt5, variantInt4);
}

TEST(ContainerVariant, CopyConstructor)
{
	// Value

	bzd::Variant<int, bool, double> variant1{static_cast<double>(3.1415)};
	EXPECT_NEAR(variant1.get<double>(), 3.1415, 0.0001);
	EXPECT_EQ(variant1.index(), 2);
	bzd::Variant<int, bool, double> variant2{variant1};
	EXPECT_NEAR(variant2.get<double>(), 3.1415, 0.0001);
	EXPECT_EQ(variant2.index(), 2);

	using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
	{
		LifetimeCounter value{};
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 0U);
		EXPECT_EQ(LifetimeCounter::destructor, 0U);
		bzd::Variant<LifetimeCounter, int> variant3{value};
		EXPECT_EQ(variant3.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 1U);
		EXPECT_EQ(LifetimeCounter::move, 0U);
		EXPECT_EQ(LifetimeCounter::destructor, 0U);
		variant3 = static_cast<int>(12);
		EXPECT_EQ(variant3.index(), 1);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 1U);
		EXPECT_EQ(LifetimeCounter::move, 0U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
	}
	EXPECT_EQ(LifetimeCounter::destructor, 2U);

	// Variant
	using LifetimeCounterB = bzd::test::LifetimeCounter<struct b>;
	{
		LifetimeCounterB value{};
		bzd::Variant<LifetimeCounterB, int> variant1{value};
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounterB::constructor, 1U);
		EXPECT_EQ(LifetimeCounterB::copy, 1U);
		EXPECT_EQ(LifetimeCounterB::move, 0U);
		EXPECT_EQ(LifetimeCounterB::destructor, 0U);
		bzd::Variant<LifetimeCounterB, int> variant2{variant1};
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(LifetimeCounterB::constructor, 1U);
		EXPECT_EQ(LifetimeCounterB::copy, 2U);
		EXPECT_EQ(LifetimeCounterB::move, 0U);
		EXPECT_EQ(LifetimeCounterB::destructor, 0U);
	}
	EXPECT_EQ(LifetimeCounterB::constructor, 1U);
	EXPECT_EQ(LifetimeCounterB::copy, 2U);
	EXPECT_EQ(LifetimeCounterB::move, 0U);
	EXPECT_EQ(LifetimeCounterB::destructor, 3U);

	// Empty variant
	using LifetimeCounterC = bzd::test::LifetimeCounter<struct c>;
	{
		bzd::Variant<LifetimeCounterC, int> variant1{};
		EXPECT_EQ(variant1.index(), 0);
		bzd::Variant<LifetimeCounterC, int> variant2{variant1};
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(LifetimeCounterC::constructor, 1U);
		EXPECT_EQ(LifetimeCounterC::copy, 1U);
		EXPECT_EQ(LifetimeCounterC::move, 0U);
		EXPECT_EQ(LifetimeCounterC::destructor, 0U);
	}
	EXPECT_EQ(LifetimeCounterC::constructor, 1U);
	EXPECT_EQ(LifetimeCounterC::copy, 1U);
	EXPECT_EQ(LifetimeCounterC::move, 0U);
	EXPECT_EQ(LifetimeCounterC::destructor, 2U);
}

TEST(ContainerVariantTrivial, CopyAssignment)
{
	{
		constexpr bzd::Variant<bool, int> variant1{static_cast<int>(42)};
		EXPECT_EQ(variant1.index(), 1);
		bzd::Variant<bool, int> variant2{static_cast<bool>(true)};
		EXPECT_EQ(variant2.index(), 0);
		variant2 = variant1;
		EXPECT_EQ(variant2.index(), 1);
	}

	{
		bzd::test::CopyOnly value{};
		bzd::Variant<int, bzd::test::CopyOnly> variant1{value};
		EXPECT_EQ(variant1.index(), 1);
		EXPECT_EQ(variant1.get<bzd::test::CopyOnly>().getCopiedCounter(), 1U);
		bzd::Variant<int, bzd::test::CopyOnly> variant2;
		EXPECT_EQ(variant2.index(), 0);
		variant2 = variant1;
		EXPECT_EQ(variant2.index(), 1);
		EXPECT_EQ(variant2.get<bzd::test::CopyOnly>().getCopiedCounter(), 2U);
	}

	{
		struct Temp
		{
		};
		bzd::test::CopyOnly value1{};
		bzd::Variant<Temp, bzd::test::CopyOnly> variant1{value1};
		EXPECT_EQ(variant1.index(), 1);
		Temp value2{};
		variant1 = value2;
		EXPECT_EQ(variant1.index(), 0);
	}
}

TEST(ContainerVariantNonTrivial, CopyAssignment)
{
	{
		using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
		bzd::Variant<LifetimeCounter, int> variant1{LifetimeCounter{}};
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
		bzd::Variant<LifetimeCounter, int> variant2{static_cast<int>(1)};
		EXPECT_EQ(variant2.index(), 1);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
		variant2 = variant1;
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 1U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
		variant1 = variant2;
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 2U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 2U);
	}
}

TEST(ContainerVariant, MoveConstructor)
{
	// Value

	bzd::Variant<int, bool, double> variant{bzd::Variant<int, bool, double>{static_cast<double>(3.1415)}};
	EXPECT_EQ(variant.index(), 2);
	EXPECT_NEAR(variant.get<double>(), 3.1415, 0.0001);

	using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
	{
		LifetimeCounter value{};
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 0U);
		EXPECT_EQ(LifetimeCounter::destructor, 0U);
		bzd::Variant<LifetimeCounter, int> variant1{bzd::move(value)};
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 0U);
	}
	EXPECT_EQ(LifetimeCounter::destructor, 2U);

	// Variant
	using LifetimeCounterB = bzd::test::LifetimeCounter<struct b>;
	{
		bzd::Variant<LifetimeCounterB, int> variant1{LifetimeCounterB{}};
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounterB::constructor, 1U);
		EXPECT_EQ(LifetimeCounterB::copy, 0U);
		EXPECT_EQ(LifetimeCounterB::move, 1U);
		EXPECT_EQ(LifetimeCounterB::destructor, 1U);
		bzd::Variant<LifetimeCounterB, int> variant2{bzd::move(variant1)};
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(LifetimeCounterB::constructor, 1U);
		EXPECT_EQ(LifetimeCounterB::copy, 0U);
		EXPECT_EQ(LifetimeCounterB::move, 2U);
		EXPECT_EQ(LifetimeCounterB::destructor, 1U);
	}
	EXPECT_EQ(LifetimeCounterB::constructor, 1U);
	EXPECT_EQ(LifetimeCounterB::copy, 0U);
	EXPECT_EQ(LifetimeCounterB::move, 2U);
	EXPECT_EQ(LifetimeCounterB::destructor, 3U);

	// Empty variant
	using LifetimeCounterC = bzd::test::LifetimeCounter<struct c>;
	{
		bzd::Variant<LifetimeCounterC, int> variant1{};
		EXPECT_EQ(variant1.index(), 0);
		bzd::Variant<LifetimeCounterC, int> variant2{bzd::move(variant1)};
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(LifetimeCounterC::constructor, 1U);
		EXPECT_EQ(LifetimeCounterC::copy, 0U);
		EXPECT_EQ(LifetimeCounterC::move, 1U);
		EXPECT_EQ(LifetimeCounterC::destructor, 0U);
	}
	EXPECT_EQ(LifetimeCounterC::constructor, 1U);
	EXPECT_EQ(LifetimeCounterC::copy, 0U);
	EXPECT_EQ(LifetimeCounterC::move, 1U);
	EXPECT_EQ(LifetimeCounterC::destructor, 2U);
}

TEST(ContainerVariantTrivial, MoveAssignment)
{
	{
		constexpr bzd::Variant<bool, int> variant1{static_cast<int>(42)};
		EXPECT_EQ(variant1.index(), 1);
		bzd::Variant<bool, int> variant2{static_cast<bool>(true)};
		EXPECT_EQ(variant2.index(), 0);
		variant2 = bzd::move(variant1);
		EXPECT_EQ(variant2.index(), 1);
	}

	{
		bzd::test::MoveOnly value{};
		bzd::Variant<int, bzd::test::MoveOnly> variant1{bzd::move(value)};
		EXPECT_EQ(variant1.index(), 1);
		EXPECT_EQ(variant1.get<bzd::test::MoveOnly>().getMovedCounter(), 1U);
		bzd::Variant<int, bzd::test::MoveOnly> variant2;
		EXPECT_EQ(variant2.index(), 0);
		variant2 = bzd::move(variant1);
		EXPECT_EQ(variant2.index(), 1);
		EXPECT_EQ(variant2.get<bzd::test::MoveOnly>().getMovedCounter(), 2U);
		variant1 = static_cast<int>(42);
		variant2 = bzd::move(variant1);
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(variant2.get<int>(), 42);
	}

	{
		struct Temp
		{
		};
		bzd::Variant<Temp, bzd::test::MoveOnly> variant1{bzd::test::MoveOnly{}};
		EXPECT_EQ(variant1.index(), 1);
		variant1 = Temp{};
		EXPECT_EQ(variant1.index(), 0);
	}
}

TEST(ContainerVariantNonTrivial, MoveAssignment)
{
	{
		using LifetimeCounter = bzd::test::LifetimeCounter<struct a>;
		bzd::Variant<LifetimeCounter, int> variant1{LifetimeCounter{}};
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
		bzd::Variant<LifetimeCounter, int> variant2{static_cast<int>(1)};
		EXPECT_EQ(variant2.index(), 1);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 1U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
		variant2 = bzd::move(variant1);
		EXPECT_EQ(variant2.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 2U);
		EXPECT_EQ(LifetimeCounter::destructor, 1U);
		variant1 = bzd::move(variant2);
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounter::constructor, 1U);
		EXPECT_EQ(LifetimeCounter::copy, 0U);
		EXPECT_EQ(LifetimeCounter::move, 3U);
		EXPECT_EQ(LifetimeCounter::destructor, 2U);
	}
}

TEST(ContainerVariant, Destructor)
{
	using LifetimeCounterA = bzd::test::LifetimeCounter<struct a>;
	using LifetimeCounterB = bzd::test::LifetimeCounter<struct b>;

	{
		bzd::Variant<LifetimeCounterA, LifetimeCounterB> variant1;
		EXPECT_EQ(variant1.index(), 0);
		EXPECT_EQ(LifetimeCounterA::constructor, 1U);
		EXPECT_EQ(LifetimeCounterA::destructor, 0U);
		EXPECT_EQ(LifetimeCounterB::constructor, 0U);
		EXPECT_EQ(LifetimeCounterB::destructor, 0U);
	}
	EXPECT_EQ(LifetimeCounterA::constructor, 1U);
	EXPECT_EQ(LifetimeCounterA::destructor, 1U);
	EXPECT_EQ(LifetimeCounterB::constructor, 0U);
	EXPECT_EQ(LifetimeCounterB::destructor, 0U);

	{
		bzd::Variant<LifetimeCounterA, LifetimeCounterB> variant(LifetimeCounterB{});
		EXPECT_EQ(variant.index(), 1);
		EXPECT_EQ(LifetimeCounterA::constructor, 1U);
		EXPECT_EQ(LifetimeCounterA::destructor, 1U);
		EXPECT_EQ(LifetimeCounterB::constructor, 1U);
		EXPECT_EQ(LifetimeCounterB::copy, 0U);
		EXPECT_EQ(LifetimeCounterB::move, 1U);
		EXPECT_EQ(LifetimeCounterB::destructor, 1U);
	}
	EXPECT_EQ(LifetimeCounterA::destructor, 1U);
	EXPECT_EQ(LifetimeCounterB::destructor, 2U);

	{
		bzd::Variant<LifetimeCounterA, LifetimeCounterB> variant(LifetimeCounterA{});
		EXPECT_EQ(variant.index(), 0);
		EXPECT_EQ(LifetimeCounterA::constructor, 2U);
		EXPECT_EQ(LifetimeCounterA::copy, 0U);
		EXPECT_EQ(LifetimeCounterA::move, 1U);
		EXPECT_EQ(LifetimeCounterA::destructor, 2U);
		EXPECT_EQ(LifetimeCounterB::constructor, 1U);
		EXPECT_EQ(LifetimeCounterB::copy, 0U);
		EXPECT_EQ(LifetimeCounterB::move, 1U);
		EXPECT_EQ(LifetimeCounterB::destructor, 2U);
	}
	EXPECT_EQ(LifetimeCounterA::destructor, 3U);
	EXPECT_EQ(LifetimeCounterB::destructor, 2U);
}

TEST(ContainerVariant, Is)
{
	bzd::Variant<int, bool, double> variantBool(static_cast<bool>(true));
	EXPECT_EQ(variantBool.index(), 1);
	EXPECT_TRUE(variantBool.is<bool>());
	EXPECT_FALSE(variantBool.is<int>());
	EXPECT_FALSE(variantBool.is<double>());
	EXPECT_FALSE(variantBool.is<char>());

	bzd::Variant<int, bool, double> variantEmpty;
	EXPECT_EQ(variantEmpty.index(), 0);
	EXPECT_TRUE(variantEmpty.is<int>());
}

TEST(ContainerVariant, Get)
{
	bzd::Variant<bool, int, double> variantInt(static_cast<int>(-12));
	EXPECT_EQ(variantInt.index(), 1);

	auto isBool = variantInt.is<bool>();
	EXPECT_FALSE(isBool);
	auto isInt = variantInt.is<int>();
	EXPECT_TRUE(isInt);
	EXPECT_EQ(variantInt.get<int>(), -12);
	variantInt.get<int>() = 42;
	EXPECT_EQ(variantInt.get<int>(), 42);
}

TEST(ContainerVariant, Match)
{
	{
		bzd::Variant<int, bool, double> variant(static_cast<double>(5.6));
		double b = 0;
		variant.match([](const int) {}, [](const bool) {}, [&](const double a) { b = a; });
		EXPECT_NEAR(b, 5.6, 0.001);
	}
	{
		bzd::Variant<int, bool, double> variant(static_cast<int>(5));
		int b = 0;
		variant.match([&](const int a) { b = a; }, [](const bool) {}, [](const double) {});
		EXPECT_EQ(b, 5);
	}
	// Match all
	{
		bzd::Variant<int, bool, double> variant(static_cast<int>(5));
		bool isHandled = false;
		variant.match([&](auto&&) { isHandled = true; });
		EXPECT_TRUE(isHandled);
	}
	// On empty variant
	{
		bzd::Variant<int, bool, double> variant{};
		bool isHandled = false;
		variant.match([&](int) { isHandled = true; });
		EXPECT_TRUE(isHandled);
	}
	// Reference
	{
		double original = 5.6;
		bzd::Variant<int, bool, double&> variant(bzd::inPlaceType<double&>, original);
		variant.match([](const int) {}, [](const bool) {}, [](double& a) { a = 3.14; });
		EXPECT_NEAR(original, 3.14, 0.001);
	}
}

TEST(ContainerVariant, GetSet)
{
	bzd::Variant<int, bool, double> variant(static_cast<double>(5.6));
	// By type
	variant.set<bool>(false);
	EXPECT_EQ(variant.index(), 1);
	EXPECT_EQ(variant.get<bool>(), false);
	// By index
	variant.set<0>(7);
	EXPECT_EQ(variant.index(), 0);
	EXPECT_EQ(variant.get<0>(), 7);
}

TEST(ContainerVariant, Emplace)
{
	bzd::Variant<int, bool, double> variant(static_cast<double>(5.6));
	EXPECT_EQ(variant.index(), 2);
	// By type
	variant.emplace<int>(5);
	EXPECT_EQ(variant.index(), 0);
	// By index
	variant.emplace<1>(false);
	EXPECT_EQ(variant.index(), 1);
}

TEST_CONSTEXPR_BEGIN(ContainerVariant, Constexpr)
{
	constexpr bzd::Variant<int, bool, double> variant;
	EXPECT_EQ(variant.index(), 0);
	EXPECT_TRUE(variant.is<int>());
	EXPECT_FALSE(variant.is<bool>());
	EXPECT_FALSE(variant.is<double>());

	constexpr bzd::Variant<int, bool, double> variantInt(static_cast<int>(45));
	EXPECT_EQ(variantInt.index(), 0);
	EXPECT_TRUE(variantInt.is<int>());
	constexpr bzd::Variant<int, bool, double> variantBool(static_cast<bool>(true));
	EXPECT_EQ(variantBool.index(), 1);
	EXPECT_TRUE(variantBool.is<bool>());
	constexpr bzd::Variant<int, bool, double> variantDouble(static_cast<double>(5.4));
	EXPECT_EQ(variantDouble.index(), 2);
	EXPECT_TRUE(variantDouble.is<double>());

	const auto isDouble = variantDouble.is<double>();
	EXPECT_TRUE(isDouble);
	EXPECT_NEAR(variantDouble.get<double>(), 5.4, 0.01);

	{
		constexpr bzd::Variant<int, bool, double> variant(static_cast<double>(5.6));
		double b = 0;
		variant.match([](const int) {}, [](const bool) {}, [&](const double a) { b = a; });
		EXPECT_NEAR(b, 5.6, 0.001);
	}

	{
		constexpr bzd::Variant<int, bool, double> variantCopied{variantDouble};
		EXPECT_EQ(variantCopied.index(), 2);
		EXPECT_TRUE(variantCopied.is<double>());
		EXPECT_NEAR(variantCopied.get<double>(), 5.4, 0.01);

		constexpr bzd::Variant<int, bool, double> variantMoved{bzd::move(variantCopied)};
		EXPECT_EQ(variantMoved.index(), 2);
		EXPECT_TRUE(variantMoved.is<double>());
		EXPECT_NEAR(variantMoved.get<double>(), 5.4, 0.01);
	}
}
TEST_CONSTEXPR_END(ContainerVariant, Constexpr)
