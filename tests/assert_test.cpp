#include "bzd/assert.h"

#include "cc_test/test.h"

TEST(Assert, Base)
{
	bzd::assert::isTrue(true);
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}
