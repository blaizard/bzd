#include "cc/bzd/utility/string/reader/from_string.hh"

#include "cc/bzd/test/test.hh"

TEST(FromString, NoArguments)
{
	const bzd::StringView str{"Hello World"};
	const auto result = fromString(str, "Hello"_csv);
	EXPECT_FALSE(result);
}

/*
TEST(FromString, SimpleArguments)
{
	const bzd::StringView str{"Number 12"};
    bzd::UInt32 a;
	const auto result = fromString(str, "Number {}"_csv, a);
	EXPECT_FALSE(result);
}
*/
