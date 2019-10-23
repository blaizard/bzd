#include "gtest/gtest.h"
#include "include/to_string.h"

TEST(ToString, Integer)
{
	bzd::String<10> str;

	bzd::toString(str, 12);
	EXPECT_EQ(str.size(), 2);
    EXPECT_STREQ(str.data(), "12");

	bzd::toString(str, -426);
	EXPECT_EQ(str.size(), 4);
    EXPECT_STREQ(str.data(), "-426");

	bzd::toString(str, 0);
	EXPECT_EQ(str.size(), 1);
    EXPECT_STREQ(str.data(), "0");

	bzd::toString(str, static_cast<long long int>(1234567890));  
	EXPECT_EQ(str.size(), 10);
    EXPECT_STREQ(str.data(), "1234567890");
}

TEST(ToString, Float)
{
	bzd::String<10> str;

	bzd::toString(str, 12.45);
    EXPECT_STREQ(str.data(), "12.45");

	bzd::toString(str, 12.50049, 5);
    EXPECT_STREQ(str.data(), "12.50049");
	bzd::toString(str, 12.50049, 4);
    EXPECT_STREQ(str.data(), "12.5005");
	bzd::toString(str, 12.50049, 3);
    EXPECT_STREQ(str.data(), "12.5");
	bzd::toString(str, 12.50049, 2);
    EXPECT_STREQ(str.data(), "12.5");
	bzd::toString(str, 12.50049, 1);
    EXPECT_STREQ(str.data(), "12.5");
	bzd::toString(str, 12.50049, 0);
    EXPECT_STREQ(str.data(), "13.");
}

TEST(ToString, ToStringBin)
{
	bzd::StringStream<128> stream;

	stream.str().clear();
	bzd::toStringBin(stream, 16);
    EXPECT_STREQ(stream.str().data(), "10000");

	stream.str().clear();
	bzd::toStringBin(stream, 0xffffff);
    EXPECT_STREQ(stream.str().data(), "111111111111111111111111");

	stream.str().clear();
	bzd::toStringBin(stream, 0xff0001);
    EXPECT_STREQ(stream.str().data(), "111111110000000000000001");
}

TEST(ToString, Overflow)
{
	{
		bzd::String<0> str;
		bzd::toString(str, 12);
		EXPECT_EQ(str.size(), 0);
		bzd::toString(str, -12);
		EXPECT_EQ(str.size(), 0);
	}

	{
		bzd::String<1> str;
		bzd::toString(str, 12);
		EXPECT_EQ(str.size(), 1);
    	EXPECT_STREQ(str.data(), "1");
		bzd::toString(str, -12);
		EXPECT_EQ(str.size(), 1);
    	EXPECT_STREQ(str.data(), "-");
	}
}
