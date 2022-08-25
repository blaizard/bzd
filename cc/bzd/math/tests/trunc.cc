#include "cc/bzd/math/trunc.hh"

#include "cc/bzd/test/test.hh"

TEST(Trunc, Base)
{
	EXPECT_EQ(bzd::trunc(0), 0);
	EXPECT_EQ(bzd::trunc(-12), -12);
	EXPECT_EQ(bzd::trunc(1), 1);
	EXPECT_NEAR(bzd::trunc(1.2), 1, 0.01);
	EXPECT_NEAR(bzd::trunc(-1.2), -1, 0.01);
	EXPECT_NEAR(bzd::trunc(-2.7), -2, 0.01);
	EXPECT_NEAR(bzd::trunc(-0.5), 0, 0.01);
	EXPECT_NEAR(bzd::trunc(0.3), 0, 0.01);
	EXPECT_NEAR(bzd::trunc(+0), +0, 0.01);
	EXPECT_NEAR(bzd::trunc(-0), -0, 0.01);
	EXPECT_EQ(bzd::trunc(-INFINITY), -INFINITY);
	EXPECT_EQ(bzd::trunc(+INFINITY), +INFINITY);
	EXPECT_NEAR(bzd::trunc(3E+33f), 3E+33f, 0.01);
	EXPECT_NEAR(bzd::trunc(3E+65), 3E+65, 0.01);
}
