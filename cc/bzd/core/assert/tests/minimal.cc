#include "cc/bzd/core/assert/minimal.hh"

#include "cc_test/test.hh"

TEST(AssertMinimal, Base)
{
	bzd::assert::isTrue(true);
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}

TEST(AssertMinimal, Message)
{
	bzd::assert::isTrue(true, "This is a message");
	EXPECT_ANY_THROW(bzd::assert::isTrue(false, "This is another message"));
}

TEST(AssertMinimal, Unreachable)
{
	EXPECT_ANY_THROW(bzd::assert::unreachable());
}