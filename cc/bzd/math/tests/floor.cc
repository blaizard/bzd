#include "cc/bzd/math/floor.hh"

#include "cc/bzd/test/test.hh"

TEST(Floor, Base)
{
	EXPECT_EQ(bzd::floor(0), 0);
	EXPECT_EQ(bzd::floor(-12), -12);
	EXPECT_EQ(bzd::floor(1), 1);
	EXPECT_NEAR(bzd::floor(1.2), 1, 0.01);
	EXPECT_NEAR(bzd::floor(-1.2), -2, 0.01);
	EXPECT_NEAR(bzd::floor(-2.7), -3, 0.01);
	EXPECT_NEAR(bzd::floor(-0.5), -1, 0.01);
	EXPECT_NEAR(bzd::floor(0.3), 0, 0.01);
}
