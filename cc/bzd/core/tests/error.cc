#include "cc/bzd/core/error.hh"

#include "cc/bzd/test/test.hh"

TEST(Error, Constructor)
{
	{
		const auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::failure, "This is a test error"_csv};
		EXPECT_EQ(error.getType(), bzd::ErrorType::failure);
		EXPECT_STREQ(error.getMessage().data(), "This is a test error");
	}
	{
		const auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::timeout, "Number {}."_csv, 12};
		EXPECT_EQ(error.getType(), bzd::ErrorType::timeout);
		EXPECT_STREQ(error.getMessage().data(), "Number 12.");
	}
	{
		const auto error =
			bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::busy, "fixed, {}, {}, {}, {}"_csv, 12, -1, true, "string"};
		EXPECT_EQ(error.getType(), bzd::ErrorType::busy);
		EXPECT_STREQ(error.getMessage().data(), "fixed, 12, -1, true, string");
	}
}

TEST(Error, MoveConstructor)
{
	{
		auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::failure, "This is a test error"_csv};
		EXPECT_STREQ(error.getMessage().data(), "This is a test error");
		const auto dest{bzd::move(error)};
		EXPECT_STREQ(dest.getMessage().data(), "This is a test error");
	}

	{
		auto error = bzd::Error{bzd::SourceLocation::current(), bzd::ErrorType::failure, "Hello {}"_csv, "world"};
		EXPECT_STREQ(error.getMessage().data(), "Hello world");
		const auto dest{bzd::move(error)};
		EXPECT_STREQ(dest.getMessage().data(), "Hello world");
	}
}
