#include "bzd/meta/choose_nth.h"
#include "bzd/meta/contains.h"
#include "bzd/meta/find.h"
#include "bzd/meta/type_list.h"
#include "bzd/meta/union.h"
#include "cc_test/test.h"

#include <typeinfo>

TEST(Meta, ChooseNth)
{
	bzd::meta::ChooseNth<0, int, double> a;
	EXPECT_EQ(typeid(a), typeid(int));

	bzd::meta::ChooseNth<1, int, double> b;
	EXPECT_EQ(typeid(b), typeid(double));

	using TypeList = bzd::meta::TypeList<int, double>;
	TypeList::ChooseNth<0> c;
	EXPECT_EQ(typeid(c), typeid(int));
	TypeList::ChooseNth<1> d;
	EXPECT_EQ(typeid(d), typeid(double));
}

TEST(Meta, Contains)
{
	const bool a = bzd::meta::Contains<int, int, double>::value;
	EXPECT_TRUE(a);

	const bool b = bzd::meta::Contains<double, int, double>::value;
	EXPECT_TRUE(b);

	const bool c = bzd::meta::Contains<bool, int, double>::value;
	EXPECT_FALSE(c);

	using TypeList = bzd::meta::TypeList<int, double>;
	const bool d = TypeList::Contains<int>::value;
	EXPECT_TRUE(d);
	const bool e = TypeList::Contains<double>::value;
	EXPECT_TRUE(e);
	const bool f = TypeList::Contains<bool>::value;
	EXPECT_FALSE(f);
}

TEST(Meta, Find)
{
	const int a = bzd::meta::Find<int, int, double>::value;
	EXPECT_EQ(a, 0);

	const int b = bzd::meta::Find<double, int, double>::value;
	EXPECT_EQ(b, 1);

	const int c = bzd::meta::Find<bool, int, double>::value;
	EXPECT_EQ(c, -1);
}
