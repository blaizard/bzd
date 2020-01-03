#include "bzd/assert.h"

#include "gtest/gtest.h"

TEST(Assert, Base)
{
	bzd::assert::isTrue(true);
	//	EXPECT_ANY_THROW(bzd::assert::isTrue(false));

	//	bzd::assert::isTrue(true, "Hello {}", 545);
}
