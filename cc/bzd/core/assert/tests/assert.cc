#include "cc/bzd/core/assert.hh"

#include "cc/bzd/test/test.hh"

TEST(Assert, Base)
{
	bzd::assert::isTrue(true, "This should not fail, {}"_csv, "no no");
	EXPECT_ANY_THROW(bzd::assert::isTrue(false));
}
