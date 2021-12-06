#include "cc/bzd/algorithm/reverse.hh"

#include "cc/bzd/container/string.hh"
#include "cc/bzd/test/test.hh"

TEST(Reverse, Base)
{
	{
		bzd::String<6> test("Hello");
		bzd::algorithm::reverse(test.begin(), test.end());
		EXPECT_STREQ(test.data(), "olleH");
	}
	{
		bzd::String<6> test("Hell");
		bzd::algorithm::reverse(test.begin(), test.end());
		EXPECT_STREQ(test.data(), "lleH");
	}
	{
		bzd::String<6> test("");
		bzd::algorithm::reverse(test.begin(), test.end());
		EXPECT_STREQ(test.data(), "");
	}
	{
		bzd::String<6> test("a");
		bzd::algorithm::reverse(test.begin(), test.end());
		EXPECT_STREQ(test.data(), "a");
	}
}
