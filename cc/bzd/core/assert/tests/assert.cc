#include "cc/bzd/core/assert.hh"

#include "cc_test/test.hh"

TEST(Assert, Base)
{
	bzd::assert::isTrue(true, CSTR("This should not fail, {}"), "no no");
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}
