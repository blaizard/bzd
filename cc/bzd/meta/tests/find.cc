#include "bzd/meta/find.h"

#include "cc_test/test.h"

TEST(Meta, Find)
{
	const int a = bzd::meta::Find<int, int, double>::value;
	EXPECT_EQ(a, 0);

	const int b = bzd::meta::Find<double, int, double>::value;
	EXPECT_EQ(b, 1);

	const int c = bzd::meta::Find<bool, int, double>::value;
	EXPECT_EQ(c, -1);
}
