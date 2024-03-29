#include "cc/bzd/utility/source_location.hh"

#include "cc/bzd/test/test.hh"

void myTest() { EXPECT_STREQ(bzd::SourceLocation::current().getFunction(), "myTest"); }

TEST(SourceLocation, Base)
{
	EXPECT_STREQ(bzd::SourceLocation::current().getFile(), "cc/bzd/utility/tests/source_location.cc");
	EXPECT_STREQ(bzd::SourceLocation::current().getFileName(), "source_location.cc");
	myTest();
}
