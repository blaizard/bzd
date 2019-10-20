#include "gtest/gtest.h"
#include "include/container/variant.h"

#include <iostream>

TEST(ContainerVariant, Constructor)
{
	bzd::Variant<int, bool, double> variant;
	bzd::Variant<int, bool, double> variantInt(static_cast<int>(45));
	bzd::Variant<int, bool, double> variantBool(static_cast<bool>(true));
	bzd::Variant<int, bool, double> variantDouble(static_cast<double>(5.4));
}

TEST(ContainerVariant, Constexpr)
{
	constexpr bzd::VariantConstexpr<int, bool, double> variant;
}

TEST(ContainerVariant, Destructor)
{
	static int constructorA = 0;
	static int destructorA = 0;
	static int constructorB = 0;
	static int destructorB = 0;

	class TypeA
	{
	public:
		TypeA() {++constructorA;}
		TypeA(const TypeA&) {++constructorA;}
		~TypeA() {++destructorA;}
	};

	class TypeB
	{
	public:
		TypeB() {++constructorB;}
		TypeB(const TypeB&) {++constructorB;}
		~TypeB() {++destructorB;}
	};

	bzd::Variant<TypeA, TypeB> variant1;
	EXPECT_EQ(constructorA, 0);
	EXPECT_EQ(destructorA, 0);
	EXPECT_EQ(constructorB, 0);
	EXPECT_EQ(destructorB, 0);

	{
		TypeB b;
		bzd::Variant<TypeA, TypeB> variant(bzd::move(b));
		EXPECT_EQ(constructorA, 0);
		EXPECT_EQ(destructorA, 0);
		EXPECT_EQ(constructorB, 2);
		EXPECT_EQ(destructorB, 0);
	}
	EXPECT_EQ(destructorB, 2);

	{
		TypeA a;
		bzd::Variant<TypeA, TypeB> variant(bzd::move(a));
		EXPECT_EQ(constructorA, 2);
		EXPECT_EQ(destructorA, 0);
		EXPECT_EQ(constructorB, 2);
		EXPECT_EQ(destructorB, 2);
	}
	EXPECT_EQ(destructorA, 2);
}

TEST(ContainerVariant, Is)
{
	bzd::Variant<int, bool, double> variantBool(static_cast<bool>(true));
	const bool isBool = variantBool.is<bool>();
	EXPECT_TRUE(isBool);
	const bool isInt = variantBool.is<int>();
	EXPECT_FALSE(isInt);
	const bool isDouble = variantBool.is<double>();
	EXPECT_FALSE(isDouble);
	const bool isChar = variantBool.is<char>();
	EXPECT_FALSE(isChar);

	bzd::Variant<int, bool, double> variantEmpty;
	const bool isEmptyBool = variantEmpty.is<bool>();
	EXPECT_FALSE(isEmptyBool);
}

TEST(ContainerVariant, Get)
{
	bzd::Variant<int, bool, double> variantBool(static_cast<bool>(true));

//	const auto a = variantBool.get<bool>();
}
