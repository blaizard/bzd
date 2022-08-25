#include "cc/bzd/math/ceil.hh"

#include "cc/bzd/test/test.hh"

TEST(Ceil, Base)
{
	EXPECT_EQ(bzd::ceil(0), 0);
	EXPECT_EQ(bzd::ceil(-12), -12);
	EXPECT_EQ(bzd::ceil(1), 1);
	EXPECT_NEAR(bzd::ceil(1.2), 2, 0.01);
	EXPECT_NEAR(bzd::ceil(-1.2), -1, 0.01);
	EXPECT_NEAR(bzd::ceil(-2.7), -2, 0.01);
	EXPECT_NEAR(bzd::ceil(-0.5), 0, 0.01);
	EXPECT_NEAR(bzd::ceil(0.3), 1, 0.01);
}
