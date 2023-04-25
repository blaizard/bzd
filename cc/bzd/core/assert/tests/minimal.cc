#include "cc/bzd/core/assert/minimal.hh"

#include "cc/bzd/platform/panic.hh"
#include "cc/bzd/test/test.hh"

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)>) { throw 42; }

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

TEST(AssertMinimal, Unreachable) { EXPECT_ANY_THROW(bzd::assert::unreachable()); }
