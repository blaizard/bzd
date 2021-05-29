#include "cc/bzd/meta/choose_nth.h"

#include "cc/bzd/meta/type_list.h"
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
