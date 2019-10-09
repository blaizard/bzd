#include "gtest/gtest.h"
#include "include/assert.h"

TEST(Assert, Base)
{
	bzd::assertTrue(true);
	EXPECT_ANY_THROW(bzd::assertTrue(false));

	//bzd::assertTrue(true, "Hello {}", 545);
}
