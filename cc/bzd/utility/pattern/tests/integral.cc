#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/to_string.hh"

TEST(ToString, Integer)
{
	bzd::String<10> str;

	{
		const auto result = bzd::toString(str.assigner(), 12);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 2U);
		EXPECT_EQ(str.size(), 2U);
		EXPECT_STREQ(str.data(), "12");
	}

	{
		const auto result = bzd::toString(str.assigner(), -426);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4U);
		EXPECT_EQ(str.size(), 4U);
		EXPECT_STREQ(str.data(), "-426");
	}

	{
		const auto result = bzd::toString(str.assigner(), 0);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1U);
		EXPECT_EQ(str.size(), 1U);
		EXPECT_STREQ(str.data(), "0");
	}

	{
		const auto result = bzd::toString(str.assigner(), static_cast<bzd::Int64>(1234567890));
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 10U);
		EXPECT_EQ(str.size(), 10U);
		EXPECT_STREQ(str.data(), "1234567890");
	}
}

TEST(ToString, Float)
{
	bzd::String<10> str;

	{
		const auto result = bzd::toString(str.assigner(), 12.45);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 5u);
		EXPECT_STREQ(str.data(), "12.45");
	}

	{
		const auto result = bzd::toString(str.assigner(), 12.50049, 5);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 8u);
		EXPECT_STREQ(str.data(), "12.50049");
	}

	{
		const auto result = bzd::toString(str.assigner(), 12.50049, 4);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 7u);
		EXPECT_STREQ(str.data(), "12.5005");
	}

	{
		const auto result = bzd::toString(str.assigner(), 12.50049, 3);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
		EXPECT_STREQ(str.data(), "12.5");
	}

	{
		const auto result = bzd::toString(str.assigner(), 12.50049, 2);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
		EXPECT_STREQ(str.data(), "12.5");
	}

	{
		const auto result = bzd::toString(str.assigner(), 12.50049, 1);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
		EXPECT_STREQ(str.data(), "12.5");
	}

	{
		const auto result = bzd::toString(str.assigner(), 12.50049, 0);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 3u);
		EXPECT_STREQ(str.data(), "13.");
	}
}

/*
TEST(ToString, ToStringBin)
{
	bzd::String<128> str;

	{
		const auto result = bzd::format::toStringBin(str.assigner(), 16);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 5u);
		EXPECT_STREQ(str.data(), "10000");
	}

	{
		const auto result = bzd::format::toStringBin(str.assigner(), 0xffffff);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 24u);
		EXPECT_STREQ(str.data(), "111111111111111111111111");
	}

	{
		const auto result = bzd::format::toStringBin(str.assigner(), 0xff0001);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 24u);
		EXPECT_STREQ(str.data(), "111111110000000000000001");
	}
}
*/

TEST(ToString, Overflow)
{
	{
		bzd::String<0> str;
		const auto result1 = bzd::toString(str.assigner(), 12);
		EXPECT_FALSE(result1);
		EXPECT_EQ(str.size(), 0U);
		const auto result2 = bzd::toString(str.assigner(), -12);
		EXPECT_FALSE(result2);
		EXPECT_EQ(str.size(), 0U);
	}

	{
		bzd::String<1> str;

		const auto result1 = bzd::toString(str.assigner(), 12);
		EXPECT_FALSE(result1);
		EXPECT_EQ(str.size(), 1U);
		EXPECT_STREQ(str.data(), "1");

		const auto result2 = bzd::toString(str.assigner(), -12);
		EXPECT_FALSE(result2);
		EXPECT_EQ(str.size(), 1U);
		EXPECT_STREQ(str.data(), "-");
	}
}
