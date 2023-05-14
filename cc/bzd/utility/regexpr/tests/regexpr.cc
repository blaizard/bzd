#include "cc/bzd/utility/regexpr/regexpr.hh"

#include "cc/bzd/test/test.hh"

#define EXPECT_RESULT_EQ(r, v)                                                                                                             \
	{                                                                                                                                      \
		const auto result = r;                                                                                                             \
		EXPECT_TRUE(result);                                                                                                               \
		EXPECT_EQ(result.value(), v);                                                                                                      \
	}

TEST(Regexpr, String)
{
	bzd::Regexpr regexpr{"abc"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("abc"_sv)), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("Hello"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("qabc"_sv)));
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("abcq"_sv)), 3u);
}

TEST(Regexpr, SpecialChar)
{
	EXPECT_RESULT_EQ(bzd::Regexpr{"a\\*c"}.match(bzd::range::makeStream("a*c"_sv)), 3u);
	EXPECT_RESULT_EQ(bzd::Regexpr{"a.c"}.match(bzd::range::makeStream("abc"_sv)), 3u);
	EXPECT_FALSE(bzd::Regexpr{"\\"}.match(bzd::range::makeStream(""_sv)));
	EXPECT_TRUE(bzd::Regexpr{"\\\\"}.match(bzd::range::makeStream("\\"_sv)));
	EXPECT_TRUE(bzd::Regexpr{"\\s"}.match(bzd::range::makeStream(" "_sv)));
	EXPECT_TRUE(bzd::Regexpr{"\\s+a"}.match(bzd::range::makeStream(" \t\na"_sv)));
}

TEST(Regexpr, Brackets)
{
	bzd::Regexpr regexpr{"a[bcde]f"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("abf"_sv)), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("adf"_sv)), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("adef"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("ad"_sv)));
}

TEST(Regexpr, BracketsRange)
{
	bzd::Regexpr regexpr{"a[b-e]f"};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("abf"_sv)), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("adf"_sv)), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("agf"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("adef"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("ad"_sv)));
	EXPECT_FALSE(bzd::Regexpr{"a[b"}.match(bzd::range::makeStream("ab"_sv)));
	EXPECT_FALSE(bzd::Regexpr{"a["}.match(bzd::range::makeStream("a"_sv)));
	EXPECT_FALSE(bzd::Regexpr{"a[]"}.match(bzd::range::makeStream("a"_sv)));
}

TEST(Regexpr, BracketsNegate)
{
	bzd::Regexpr regexpr{"a[^bd]f"};
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("abf"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("adf"_sv)));
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("agf"_sv)), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("adef"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("ad"_sv)));
	EXPECT_FALSE(bzd::Regexpr{"a[b"}.match(bzd::range::makeStream("ab"_sv)));
	EXPECT_FALSE(bzd::Regexpr{"a["}.match(bzd::range::makeStream("a"_sv)));
	EXPECT_FALSE(bzd::Regexpr{"a[]"}.match(bzd::range::makeStream("a"_sv)));
}

TEST(Regexpr, Star)
{
	bzd::Regexpr regexpr{"a*c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("aaaaac"_sv)), 6u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("c"_sv)), 1u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("acb"_sv)), 2u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("cb"_sv)), 1u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("bc"_sv)));
	EXPECT_RESULT_EQ(bzd::Regexpr{"a*b*"}.match(bzd::range::makeStream("ab"_sv)), 2u);
	EXPECT_FALSE(bzd::Regexpr{"*"}.match(bzd::range::makeStream(""_sv)));
	EXPECT_FALSE(bzd::Regexpr{"*"}.match(bzd::range::makeStream("*"_sv)));
}

TEST(Regexpr, StarBrackets)
{
	bzd::Regexpr regexpr{"a[0-9]*c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("a1c"_sv)), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("a0123456789c"_sv)), 12u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("ac"_sv)), 2u);
}

TEST(Regexpr, Plus)
{
	bzd::Regexpr regexpr{"a+c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("aaaaac"_sv)), 6u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("c"_sv)));
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("acb"_sv)), 2u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("cb"_sv)));
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("bc"_sv)));
	EXPECT_RESULT_EQ(bzd::Regexpr{"a+b+"}.match(bzd::range::makeStream("ab"_sv)), 2u);
	EXPECT_FALSE(bzd::Regexpr{"+"}.match(bzd::range::makeStream(""_sv)));
	EXPECT_FALSE(bzd::Regexpr{"+"}.match(bzd::range::makeStream("+"_sv)));
}

TEST(Regexpr, PlusBrackets)
{
	bzd::Regexpr regexpr{"a[0-9]+c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("a1c"_sv)), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("a0123456789c"_sv)), 12u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("ac"_sv)));
}

TEST(Regexpr, QuestionMark)
{
	bzd::Regexpr regexpr{"a?c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("ac"_sv)), 2u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("c"_sv)), 1u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("acb"_sv)), 2u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::makeStream("cb"_sv)), 1u);
	EXPECT_FALSE(regexpr.match(bzd::range::makeStream("bc"_sv)));
	EXPECT_RESULT_EQ(bzd::Regexpr{"a?b?"}.match(bzd::range::makeStream("ab"_sv)), 2u);
	EXPECT_FALSE(bzd::Regexpr{"?"}.match(bzd::range::makeStream(""_sv)));
	EXPECT_FALSE(bzd::Regexpr{"?"}.match(bzd::range::makeStream("?"_sv)));
}