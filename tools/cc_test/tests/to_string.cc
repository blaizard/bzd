#include "cc_test/test.h"

TEST(Test, integer)
{
	{
		bzd::test::impl::Value v{static_cast<int>(42)};
		EXPECT_STREQ(v.valueToString(), "42");
	}
	{
		bzd::test::impl::Value v{static_cast<int>(0)};
		EXPECT_STREQ(v.valueToString(), "0");
	}
	{
		bzd::test::impl::Value v{static_cast<int>(-1)};
		EXPECT_STREQ(v.valueToString(), "-1");
	}
}
