#include "cc/bzd/utility/pattern/reader/pattern.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/reader/integral.hh"

#include <iostream>

class TestAssert : public bzd::reader::impl::Schema
{
public:
	static void onError(const bzd::StringView& str) { ::std::cout << "Error at: " << str.data() << ::std::endl; }
};

using TestAdapater = bzd::pattern::impl::Adapter<TestAssert>;

TEST(PatternFromString, NoArguments)
{
	const bzd::StringView str{"Hello World"};
	const auto result = bzd::fromString(str, "Hello"_csv);
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 5u);

	const auto result2 = bzd::fromString(str, "World"_csv);
	EXPECT_FALSE(result2);
}

TEST(PatternFromString, MatcherMetadata)
{
	{
		bzd::String<12> string{""};
		auto appender = string.appender();
		EXPECT_TRUE(bzd::concepts::matcherMetadata<decltype(appender)>);
	}
	EXPECT_FALSE(bzd::concepts::matcherMetadata<int>);
	EXPECT_FALSE(bzd::concepts::matcherMetadata<unsigned int&>);
}

TEST(PatternFromString, SimpleArguments)
{
	{
		bzd::UInt32 a;
		const auto result = bzd::fromString("Hello 12"_sv, "Hello {}"_csv, a);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 8u);
		EXPECT_EQ(a, 12u);
	}
	{
		bzd::UInt32 a;
		const auto result = bzd::fromString("World 12"_sv, "[A-Za-z]+ {}"_csv, a);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 8u);
		EXPECT_EQ(a, 12u);
	}
	{
		bzd::String<12u> hello;
		const auto result1 = bzd::fromString("Hello World"_sv, "Hello {:.+}"_csv, hello.assigner());
		EXPECT_TRUE(result1);
		EXPECT_EQ(result1.value(), 11u);
		EXPECT_STREQ(hello.data(), "World");

		const auto result2 = bzd::fromString("Hello WORld"_sv, "Hello {:[A-Z]+}"_csv, hello.assigner());
		EXPECT_TRUE(result2);
		EXPECT_EQ(result2.value(), 9u);
		EXPECT_STREQ(hello.data(), "WOR");
	}
}
