#include "cc/bzd/core/error.hh"

#include "cc/bzd/test/test.hh"

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
		EXPECT_STREQ(error.getMessage().data(), "fixed, 12, -1, true, string");
	}
}

TEST(Error, MoveConstructor)
{
	{
		auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::failure, CSTR("This is a test error")};
		EXPECT_STREQ(error.getMessage().data(), "This is a test error");
		const auto dest{bzd::move(error)};
		EXPECT_STREQ(dest.getMessage().data(), "This is a test error");
	}

	{
		auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::failure, CSTR("Hello {}"), "world"};
		EXPECT_STREQ(error.getMessage().data(), "Hello world");
		const auto dest{bzd::move(error)};
		EXPECT_STREQ(dest.getMessage().data(), "Hello world");
	}
}
