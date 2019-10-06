#include "gtest/gtest.h"
#include "include/format.h"

TEST(Format, Integer)
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
}

TEST(Format, StringFormat)
{
	bzd::String<32> str;

	bzd::format::toString(str, "Hello");
	EXPECT_STREQ(str.data(), "Hello");

	bzd::format::toString(str, "Hello {i}", 42);
	EXPECT_STREQ(str.data(), "Hello 42");

	bzd::format::toString(str, "Hello {{");
	EXPECT_STREQ(str.data(), "Hello {");

	bzd::format::toString(str, "{{");
	EXPECT_STREQ(str.data(), "{");

	bzd::format::toString(str, "Hello {{ {i", 42);
	EXPECT_STREQ(str.data(), "Hello { 42");

	// to test overflows
}
