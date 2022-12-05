#include "cc/bzd/core/serialization/types/integral.hh"

#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/test/test.hh"

TEST(Integral, Boolean)
{
	bzd::String<20u> string;

	{
		const auto size = bzd::serialize(string, true);
		EXPECT_EQ(size, 1u);
		EXPECT_EQ(string[0u], '\x01');
		EXPECT_EQ(string.size(), 1u);
	}

	{
		const auto size = bzd::serialize(string, false);
		EXPECT_EQ(size, 1u);
		EXPECT_EQ(string[1u], '\x00');
		EXPECT_EQ(string.size(), 2u);
	}
}

TEST(Integral, UInt32)
{
	bzd::String<20u> string;

	{
		const auto size = bzd::serialize(string, bzd::UInt32{0x12345678});
		EXPECT_EQ(size, 4u);
		EXPECT_EQ(string[0], '\x78');
		EXPECT_EQ(string[1], '\x56');
		EXPECT_EQ(string[2], '\x34');
		EXPECT_EQ(string[3], '\x12');
		EXPECT_EQ(string.size(), 4u);
	}
}
