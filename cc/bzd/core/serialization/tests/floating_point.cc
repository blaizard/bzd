#include "cc/bzd/core/serialization/types/floating_point.hh"

#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/test/test.hh"

TEST(FloatingPoint, Float32)
{
	bzd::String<20u> string;

	{
		bzd::serialize(string.assigner(), bzd::Float32{3.14});
		EXPECT_EQ(string.size(), 4u);
		EXPECT_STREQ(string.data(), "\xc3\xf5\x48\x40");
	}

	{
		bzd::String<1u> shortString;
		bzd::serialize(shortString.assigner(), bzd::Float32{3.14});
		EXPECT_EQ(shortString.size(), 1u);
		EXPECT_STREQ(shortString.data(), "\xc3");
	}
}

TEST(FloatingPoint, Float64)
{
	bzd::String<20u> string;

	{
		bzd::serialize(string.assigner(), bzd::Float64{3.14});
		EXPECT_EQ(string.size(), 8u);
		EXPECT_STREQ(string.data(), "\x1f\x85\xeb\x51\xb8\x1e\x09\x40");
	}

	{
		bzd::String<1u> shortString;
		bzd::serialize(shortString.assigner(), bzd::Float64{3.14});
		EXPECT_EQ(shortString.size(), 1u);
		EXPECT_STREQ(shortString.data(), "\x1f");
	}
}
