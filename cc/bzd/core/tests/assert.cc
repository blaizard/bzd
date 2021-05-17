#include "bzd/core/assert.h"

#include "cc_test/test.h"

TEST(Assert, Base)
{
	bzd::assert::isTrue(true, "This should not fail, {}"_sv, "no no");
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}
