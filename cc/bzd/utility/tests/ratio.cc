#include "cc/bzd/utility/ratio.hh"

#include "cc/bzd/test/test.hh"

TEST(Ratio, Base)
{
	using MyRatio = bzd::Ratio<42, 34>;
	EXPECT_EQ(MyRatio::num, 42U);
	EXPECT_EQ(MyRatio::den, 34U);

	using MyRatioNum = bzd::Ratio<14>;
	EXPECT_EQ(MyRatioNum::num, 14U);
	EXPECT_EQ(MyRatioNum::den, 1U);
}
