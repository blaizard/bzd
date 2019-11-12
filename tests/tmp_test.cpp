#include "gtest/gtest.h"
#include "include/tmp/choose_nth.h"
#include "include/tmp/contains.h"
#include "include/tmp/find.h"
#include "include/tmp/union.h"
#include "include/tmp/type_list.h"

TEST(TMP, ChooseNth)
{
	bzd::tmp::ChooseNth<0, int, double> a;
	EXPECT_EQ(typeid(a), typeid(int));

	bzd::tmp::ChooseNth<1, int, double> b;
	EXPECT_EQ(typeid(b), typeid(double));

	using TypeList = bzd::tmp::TypeList<int, double>;
	TypeList::ChooseNth<0> c;
	EXPECT_EQ(typeid(c), typeid(int));
	TypeList::ChooseNth<1> d;
	EXPECT_EQ(typeid(d), typeid(double));
}

TEST(TMP, Contains)
{
	const bool a = bzd::tmp::Contains<int, int, double>::value;
	EXPECT_TRUE(a);

	const bool b = bzd::tmp::Contains<double, int, double>::value;
	EXPECT_TRUE(b);

	const bool c = bzd::tmp::Contains<bool, int, double>::value;
	EXPECT_FALSE(c);

	using TypeList = bzd::tmp::TypeList<int, double>;
	const bool d = TypeList::Contains<int>::value;
	EXPECT_TRUE(d);
	const bool e = TypeList::Contains<double>::value;
	EXPECT_TRUE(e);
	const bool f = TypeList::Contains<bool>::value;
	EXPECT_FALSE(f);
}

TEST(TMP, Find)
{
	const int a = bzd::tmp::Find<int, int, double>::value;
	EXPECT_EQ(a, 0);

	const int b = bzd::tmp::Find<double, int, double>::value;
	EXPECT_EQ(b, 1);

	const int c = bzd::tmp::Find<bool, int, double>::value;
	EXPECT_EQ(c, -1);
}

TEST(TMP, Union)
{
	bzd::tmp::Union<int, bool, float> test;

	test.get<int>() = 21;
	EXPECT_EQ(test.get<int>(), 21);
	test.get<bool>() = true;
	EXPECT_TRUE(test.get<bool>());
	test.get<float>() = 3.435;
	EXPECT_NEAR(test.get<float>(), 3.435, 0.0001);

	// Constructor
	bzd::tmp::Union<int, bool, float> testInt(static_cast<int>(42));
	EXPECT_EQ(testInt.get<int>(), 42);

	const bzd::tmp::Union<int, bool, float> constTest(static_cast<bool>(true));
	EXPECT_TRUE(constTest.get<bool>());

	constexpr bzd::tmp::UnionConstexpr<int, bool, float> constexprTest(static_cast<float>(32.5));
	EXPECT_NEAR(constexprTest.get<float>(), 32.5, 0.001);

	// Constexpr Copy Constructor
	constexpr auto constexprTest2(constexprTest);
	constexpr auto constexprTest3 = constexprTest2;
	EXPECT_NEAR(constexprTest3.get<float>(), 32.5, 0.001);

	// Complex types
	bzd::tmp::Union<bool, std::string> testComplex(std::string("Hello"));
	EXPECT_STREQ(testComplex.get<std::string>().c_str(), "Hello");
}
