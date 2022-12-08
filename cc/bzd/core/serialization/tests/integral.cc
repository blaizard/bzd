#include "cc/bzd/core/serialization/types/integral.hh"

#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/test/test.hh"

TEST(Integral, Boolean)
{
	bzd::String<20u> string;

	{
		bzd::serialize(string.appender(), true);
		EXPECT_EQ(string[0u], '\x01');
		EXPECT_EQ(string.size(), 1u);
	}

	{
		bzd::serialize(string.appender(), false);
		EXPECT_EQ(string[1u], '\x00');
		EXPECT_EQ(string.size(), 2u);
	}
}

TEST(Integral, UInt32)
{
	bzd::String<20u> string;

	{
		bzd::serialize(string.assigner(), bzd::UInt32{0x12345678});
		EXPECT_EQ(string.size(), 4u);
		EXPECT_STREQ(string.data(), "\x78\x56\x34\x12");
	}

	{
		bzd::String<1u> shortString;
		bzd::serialize(shortString.assigner(), bzd::UInt32{0x12345678});
		EXPECT_EQ(shortString.size(), 1u);
		EXPECT_STREQ(shortString.data(), "\x78");
	}
}
