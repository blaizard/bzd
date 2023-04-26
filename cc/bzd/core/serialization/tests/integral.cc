#include "cc/bzd/core/serialization/types/integral.hh"

#include "cc/bzd/test/test.hh"

TEST(Integral, Boolean)
{
	bzd::String<20u> string;

	{
		const auto result = bzd::serialize(string.appender(), true);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1u);
		EXPECT_EQ(string[0u], '\x01');
		EXPECT_EQ(string.size(), 1u);
	}

	{
		bzd::Bool value{};
		const auto result = bzd::deserialize(string, value);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1u);
		EXPECT_TRUE(value);
	}

	{
		const auto result = bzd::serialize(string.appender(), false);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1u);
		EXPECT_EQ(string[1u], '\x00');
		EXPECT_EQ(string.size(), 2u);
	}

	{
		const auto result = bzd::serialize(string, false);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1u);
		EXPECT_EQ(string[0u], '\x00');
		EXPECT_EQ(string.size(), 2u);
	}
}

TEST(Integral, UInt32)
{
	bzd::String<20u> string;

	{
		const auto result = bzd::serialize(string.assigner(), bzd::UInt32{0x12345678});
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
		EXPECT_EQ(string.size(), 4u);
		EXPECT_STREQ(string.data(), "\x78\x56\x34\x12");
	}

	{
		bzd::UInt32 value{};
		const auto result = bzd::deserialize(string, value);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
		EXPECT_EQ(value, bzd::UInt32{0x12345678});
	}

	{
		bzd::String<1u> shortString;
		const auto result1 = bzd::serialize(shortString.assigner(), bzd::UInt32{0x12345678});
		EXPECT_FALSE(result1);
		EXPECT_EQ(shortString.size(), 1u);
		EXPECT_STREQ(shortString.data(), "\x78");

		bzd::UInt32 value{};
		const auto result2 = bzd::deserialize(shortString, value);
		EXPECT_FALSE(result2);
	}
}
