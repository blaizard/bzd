#include "cc/bzd/core/assert.hh"
#include "cc/bzd/platform/panic.hh"

#include "cc/bzd/test/test.hh"

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)>)
{
	throw 42;
}

TEST(Assert, Base)
{
	bzd::assert::isTrue(true, "This should not fail, {}"_csv, "no no");
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}
