#include "gtest/gtest.h"
#include "include/format.h"

TEST(Format, Base)
{
	bzd::String<10> str;

	bzd::format::toString(str, 12);
	EXPECT_EQ(str.size(), 2);
    EXPECT_STREQ(str.data(), "12");

	bzd::format::toString(str, -426);
	EXPECT_EQ(str.size(), 4);
    EXPECT_STREQ(str.data(), "-426");

	bzd::format::toString(str, 0);
	EXPECT_EQ(str.size(), 1);
    EXPECT_STREQ(str.data(), "0");

	bzd::format::toString(str, static_cast<long long int>(1234567890));
	EXPECT_EQ(str.size(), 10);
    EXPECT_STREQ(str.data(), "1234567890");
}

TEST(Format, Overflow)
{
	{
		bzd::String<0> str;
		bzd::format::toString(str, 12);
		EXPECT_EQ(str.size(), 0);
		bzd::format::toString(str, -12);
		EXPECT_EQ(str.size(), 0);
	}

	{
		bzd::String<1> str;
		bzd::format::toString(str, 12);
		EXPECT_EQ(str.size(), 1);
    	EXPECT_STREQ(str.data(), "2");
		bzd::format::toString(str, -12);
		EXPECT_EQ(str.size(), 1);
    	EXPECT_STREQ(str.data(), "2");
	}

	bzd::format::format("Helllo {10} dfdsf");
	EXPECT_EQ(false, true);
}
