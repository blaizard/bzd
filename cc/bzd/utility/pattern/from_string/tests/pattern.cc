#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/from_string.hh"

TEST(PatternFromString, NoArguments)
{
	const bzd::StringView str{"Hello World"};
	const auto result = bzd::fromString(str, "Hello"_csv);
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 5u);

	const auto result2 = bzd::fromString(str, "World"_csv);
	EXPECT_FALSE(result2);
}

TEST(PatternFromString, SimpleArguments)
{
	{
		bzd::UInt32 a{};
		const auto result = bzd::fromString("Hello 12"_sv, "Hello {}"_csv, a);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 8u);
		EXPECT_EQ(a, 12u);
	}
	{
		bzd::UInt32 a{};
		const auto result = bzd::fromString("World 12"_sv, "[A-Za-z]+ {}"_csv, a);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 8u);
		EXPECT_EQ(a, 12u);
	}
	{
		bzd::UInt32 a{};
		const auto result = bzd::fromString("World 0xa2"_sv, "[A-Za-z]+ 0x{:x}"_csv, a);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 10u);
		EXPECT_EQ(a, 162u);
	}
	{
		bzd::String<12u> world;
		const auto result1 = bzd::fromString("Hello World"_sv, "Hello {:.+}"_csv, world.assigner());
		EXPECT_TRUE(result1);
		EXPECT_EQ(result1.value(), 11u);
		EXPECT_STREQ(world.data(), "World");

		bzd::String<12u> hello;
		const auto result2 = bzd::fromString("Hello WORld"_sv, "{:[a-zA-Z]+} {:[A-Z]+}"_csv, hello.assigner(), world.appender());
		EXPECT_TRUE(result2);
		EXPECT_EQ(result2.value(), 9u);
		EXPECT_STREQ(hello.data(), "Hello");
		EXPECT_STREQ(world.data(), "WorldWOR");
	}
}
