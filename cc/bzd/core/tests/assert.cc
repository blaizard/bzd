#include "bzd/core/assert.h"

#include "cc_test/test.h"

TEST(Assert, Base)
{
	bzd::assert::isTrue(true, CSTR("This should not fail, {}"), "no no");
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}
