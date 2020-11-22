#include "bzd/core/units.h"

#include "cc_test/test.h"
#include <iostream>

TEST(Units, length)
{
	bzd::units::Meter m{1200};

	EXPECT_EQ(m.get(), 1200);
	EXPECT_EQ(bzd::units::Decimeter{m}.get(), 12000);
	EXPECT_EQ(bzd::units::Centimeter{m}.get(), 120000);
	EXPECT_EQ(bzd::units::Millimeter{m}.get(), 1200000);
	EXPECT_EQ(bzd::units::Kilometer{m}.get(), 1);
	EXPECT_EQ(bzd::units::Hectometer{m}.get(), 12);
	EXPECT_EQ(bzd::units::Decameter{m}.get(), 120);	
}

TEST(Units, time)
{
	bzd::units::Day d{1};

	EXPECT_EQ(d.get(), 1);
	EXPECT_EQ(bzd::units::Hour{d}.get(), 24);
	EXPECT_EQ(bzd::units::Minute{d}.get(), 1440);
	EXPECT_EQ(bzd::units::Second{d}.get(), 86400);
	EXPECT_EQ(bzd::units::Millisecond{d}.get(), 86400000);
}
