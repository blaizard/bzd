#include "cc/bzd/utility/pattern/formatter/integral.hh"

#include "cc/bzd/test/test.hh"

TEST(ToString, Integer)
{
	bzd::String<10> str;

	str.clear();
	toString(str, 12);
	EXPECT_EQ(str.size(), 2U);
	EXPECT_STREQ(str.data(), "12");

	str.clear();
	toString(str, -426);
	EXPECT_EQ(str.size(), 4U);
	EXPECT_STREQ(str.data(), "-426");

	str.clear();
	toString(str, 0);
	EXPECT_EQ(str.size(), 1U);
	EXPECT_STREQ(str.data(), "0");

	str.clear();
	toString(str, static_cast<bzd::Int64>(1234567890));
	EXPECT_EQ(str.size(), 10U);
	EXPECT_STREQ(str.data(), "1234567890");
}

TEST(ToString, Float)
{
	bzd::String<10> str;

	str.clear();
	toString(str, 12.45);
	EXPECT_STREQ(str.data(), "12.45");

	str.clear();
	toString(str, 12.50049, 5);
	EXPECT_STREQ(str.data(), "12.50049");

	str.clear();
	toString(str, 12.50049, 4);
	EXPECT_STREQ(str.data(), "12.5005");

	str.clear();
	toString(str, 12.50049, 3);
	EXPECT_STREQ(str.data(), "12.5");

	str.clear();
	toString(str, 12.50049, 2);
	EXPECT_STREQ(str.data(), "12.5");

	str.clear();
	toString(str, 12.50049, 1);
	EXPECT_STREQ(str.data(), "12.5");

	str.clear();
	toString(str, 12.50049, 0);
	EXPECT_STREQ(str.data(), "13.");
}

TEST(ToString, ToStringBin)
{
	bzd::String<128> str;

	str.clear();
	bzd::format::toStringBin(str, 16);
	EXPECT_STREQ(str.data(), "10000");

	str.clear();
	bzd::format::toStringBin(str, 0xffffff);
	EXPECT_STREQ(str.data(), "111111111111111111111111");

	str.clear();
	bzd::format::toStringBin(str, 0xff0001);
	EXPECT_STREQ(str.data(), "111111110000000000000001");
}

TEST(ToString, Overflow)
{
	{
		bzd::String<0> str;
		toString(str, 12);
		EXPECT_EQ(str.size(), 0U);
		toString(str, -12);
		EXPECT_EQ(str.size(), 0U);
	}

	{
		bzd::String<1> str;

		str.clear();
		toString(str, 12);
		EXPECT_EQ(str.size(), 1U);
		EXPECT_STREQ(str.data(), "1");

		str.clear();
		toString(str, -12);
		EXPECT_EQ(str.size(), 1U);
		EXPECT_STREQ(str.data(), "-");
	}
}
