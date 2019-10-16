#include "gtest/gtest.h"
#include "include/template_metaprogramming.h"

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
