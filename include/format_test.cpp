#include "gtest/gtest.h"
#include "include/format.h"

TEST(Format, StringFormat)
{
	bzd::String<32> str;

	bzd::format(str, "Hello");
	EXPECT_STREQ(str.data(), "Hello");

	// Explicit type

	bzd::format(str, "Hello {:i}", 42);
	EXPECT_STREQ(str.data(), "Hello 42");

	bzd::format(str, "Hello {{");
	EXPECT_STREQ(str.data(), "Hello {");

	bzd::format(str, "{{");
	EXPECT_STREQ(str.data(), "{");

	bzd::format(str, "Hello {{ {:i}", 42);
	EXPECT_STREQ(str.data(), "Hello { 42");

	bzd::format(str, "Hello {:i} {:i} {:i}", 1, 2, 3);
	EXPECT_STREQ(str.data(), "Hello 1 2 3");

	// Implicit

	bzd::format(str, "Hello {}", 1);
	EXPECT_STREQ(str.data(), "Hello 1");

	bzd::format(str, "Hello {}", "World");
	EXPECT_STREQ(str.data(), "Hello World");

	// to test overflows
}
