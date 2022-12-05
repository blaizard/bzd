#include "cc/bzd/core/serialization/types/integral.hh"

#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/test/test.hh"

TEST(Integral, Boolean)
{
	bzd::String<128u> string;

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
