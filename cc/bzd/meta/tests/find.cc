#include "cc/bzd/meta/find.hh"

#include "cc/bzd/test/test.hh"

TEST(Meta, Find)
{
	const int a = bzd::meta::Find<int, int, double>::value;
	EXPECT_EQ(a, 0);

	const int b = bzd::meta::Find<double, int, double>::value;
	EXPECT_EQ(b, 1);

	const int c = bzd::meta::Find<bool, int, double>::value;
	EXPECT_EQ(c, -1);
}
