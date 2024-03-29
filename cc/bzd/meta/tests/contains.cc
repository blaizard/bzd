#include "cc/bzd/meta/contains.hh"

#include "cc/bzd/meta/type_list.hh"
#include "cc/bzd/test/test.hh"

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
