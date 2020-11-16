#include "bzd/utility/source_location.h"

#include "cc_test/test.h"

void myTest()
{
	EXPECT_STREQ(bzd::SourceLocation::current().getFunction(), "myTest");
}

TEST(SourceLocation, Base)
{
	EXPECT_EQ(bzd::SourceLocation::current().getLine(), 12);
	EXPECT_STREQ(bzd::SourceLocation::current().getFile(), "cc/bzd/utility/tests/source_location.cc");
	myTest();
}
