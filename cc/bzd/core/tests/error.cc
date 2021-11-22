#include "cc/bzd/core/error.hh"

#include "cc_test/test.hh"

TEST(Error, Constructor)
{
	{
		const auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::failure, CSTR("This is a test error")};
		EXPECT_EQ(error.getType(), bzd::ErrorType::failure);
		EXPECT_STREQ(error.getMessage().data(), "This is a test error");
	}
	{
		const auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::timeout, CSTR("Number {}."), 12};
		EXPECT_EQ(error.getType(), bzd::ErrorType::timeout);
		EXPECT_STREQ(error.getMessage().data(), "Number 12.");
	}
	{
		const auto error =
			bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::busy, CSTR("fixed, {}, {}, {}, {}"), 12, -1, true, "string"};
		EXPECT_EQ(error.getType(), bzd::ErrorType::busy);
		EXPECT_STREQ(error.getMessage().data(), "fixed, 12, -1, 1, string");
	}
}
