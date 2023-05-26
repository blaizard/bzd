#include "cc/bzd/utility/regexp/regexp.hh"

#include "cc/bzd/test/test.hh"

#define EXPECT_RESULT_EQ(r, v)                                                                                                             \
	{                                                                                                                                      \
		const auto regexpResult_ = r;                                                                                                      \
		EXPECT_TRUE(regexpResult_);                                                                                                        \
		EXPECT_EQ(regexpResult_.value(), v);                                                                                               \
	}

TEST(Regexp, String)
{
	bzd::Regexp regexp{"abc"_sv};
	EXPECT_RESULT_EQ(regexp.match("abc"), 3u);
	EXPECT_FALSE(regexp.match("Hello"));
	EXPECT_FALSE(regexp.match("qabc"));
	EXPECT_FALSE(regexp.match("ab"));
	EXPECT_RESULT_EQ(regexp.match("abcq"), 3u);
}

TEST(Regexp, SpecialChar)
{
	EXPECT_RESULT_EQ(bzd::Regexp{"a\\*c"}.match("a*c"_sv), 3u);
	EXPECT_RESULT_EQ(bzd::Regexp{"a.c"}.match("abc"_sv), 3u);
	EXPECT_FALSE(bzd::Regexp{"\\"}.match(""_sv));
	EXPECT_TRUE(bzd::Regexp{"\\\\"}.match("\\"_sv));
	EXPECT_TRUE(bzd::Regexp{"\\s"}.match(" "_sv));
	EXPECT_TRUE(bzd::Regexp{"\\w"}.match("s"_sv));
	EXPECT_FALSE(bzd::Regexp{"\\w"}.match(" "_sv));
	EXPECT_TRUE(bzd::Regexp{"\\s+a"}.match(" \t\na"_sv));
}

TEST(Regexp, Brackets)
{
	bzd::Regexp regexp{"a[bcde]f"_sv};
	EXPECT_RESULT_EQ(regexp.match("abf"_sv), 3u);
	EXPECT_RESULT_EQ(regexp.match("adf"_sv), 3u);
	EXPECT_FALSE(regexp.match("azf"_sv));
	EXPECT_FALSE(regexp.match("adef"_sv));
	EXPECT_FALSE(regexp.match("ad"_sv));
}

TEST(Regexp, BracketsRange)
{
	bzd::Regexp regexp{"a[b-e]f"};
	EXPECT_RESULT_EQ(regexp.match("abf"_sv), 3u);
	EXPECT_RESULT_EQ(regexp.match("adf"_sv), 3u);
	EXPECT_FALSE(regexp.match("agf"_sv));
	EXPECT_FALSE(regexp.match("adef"_sv));
	EXPECT_FALSE(regexp.match("ad"_sv));
	EXPECT_FALSE(bzd::Regexp{"a[b"}.match("ab"_sv));
	EXPECT_FALSE(bzd::Regexp{"a["}.match("a"_sv));
	EXPECT_FALSE(bzd::Regexp{"a[]"}.match("a"_sv));
}

TEST(Regexp, BracketsNegate)
{
	bzd::Regexp regexp{"a[^bd]f"};
	EXPECT_FALSE(regexp.match("abf"_sv));
	EXPECT_FALSE(regexp.match("adf"_sv));
	EXPECT_RESULT_EQ(regexp.match("agf"_sv), 3u);
	EXPECT_FALSE(regexp.match("adef"_sv));
	EXPECT_FALSE(regexp.match("ad"_sv));
	EXPECT_FALSE(bzd::Regexp{"a[b"}.match("ab"_sv));
	EXPECT_FALSE(bzd::Regexp{"a["}.match("a"_sv));
	EXPECT_FALSE(bzd::Regexp{"a[]"}.match("a"_sv));
}

TEST(Regexp, Star)
{
	bzd::Regexp regexp{"a*c"_sv};
	EXPECT_RESULT_EQ(regexp.match("aaaaac"_sv), 6u);
	EXPECT_RESULT_EQ(regexp.match("c"_sv), 1u);
	EXPECT_RESULT_EQ(regexp.match("acb"_sv), 2u);
	EXPECT_RESULT_EQ(regexp.match("cb"_sv), 1u);
	EXPECT_FALSE(regexp.match("bc"_sv));
	EXPECT_RESULT_EQ(bzd::Regexp{"a*b*"}.match("ab"_sv), 2u);
	EXPECT_FALSE(bzd::Regexp{"*"}.match(""_sv));
	EXPECT_FALSE(bzd::Regexp{"*"}.match("*"_sv));
}

TEST(Regexp, StarBrackets)
{
	bzd::Regexp regexp{"a[0-9]*c"_sv};
	EXPECT_RESULT_EQ(regexp.match("a1c"_sv), 3u);
	EXPECT_RESULT_EQ(regexp.match("a0123456789c"_sv), 12u);
	EXPECT_RESULT_EQ(regexp.match("ac"_sv), 2u);
}

TEST(Regexp, Plus)
{
	bzd::Regexp regexp{"a+c"_sv};
	EXPECT_RESULT_EQ(regexp.match("aaaaac"_sv), 6u);
	EXPECT_FALSE(regexp.match("c"_sv));
	EXPECT_RESULT_EQ(regexp.match("acb"_sv), 2u);
	EXPECT_FALSE(regexp.match("cb"_sv));
	EXPECT_FALSE(regexp.match("bc"_sv));
	EXPECT_RESULT_EQ(bzd::Regexp{"a+b+"}.match("ab"_sv), 2u);
	EXPECT_FALSE(bzd::Regexp{"+"}.match(""_sv));
	EXPECT_FALSE(bzd::Regexp{"+"}.match("+"_sv));
}

TEST(Regexp, PlusBrackets)
{
	bzd::Regexp regexp{"a[0-9]+c"_sv};
	EXPECT_RESULT_EQ(regexp.match("a1c"_sv), 3u);
	EXPECT_RESULT_EQ(regexp.match("a0123456789c"_sv), 12u);
	EXPECT_FALSE(regexp.match("ac"_sv));
}

TEST(Regexp, QuestionMark)
{
	bzd::Regexp regexp{"a?c"_sv};
	EXPECT_RESULT_EQ(regexp.match("ac"_sv), 2u);
	EXPECT_RESULT_EQ(regexp.match("c"_sv), 1u);
	EXPECT_RESULT_EQ(regexp.match("acb"_sv), 2u);
	EXPECT_RESULT_EQ(regexp.match("cb"_sv), 1u);
	EXPECT_FALSE(regexp.match("bc"_sv));
	EXPECT_RESULT_EQ(bzd::Regexp{"a?b?"}.match("ab"_sv), 2u);
	EXPECT_FALSE(bzd::Regexp{"?"}.match(""_sv));
	EXPECT_FALSE(bzd::Regexp{"?"}.match("?"_sv));
}

TEST(Regexp, Capture)
{
	{
		bzd::Regexp regexp{"abc"_sv};
		bzd::String<12> string;
		const auto result = regexp.capture("abc", string.assigner());
		EXPECT_RESULT_EQ(result, 3u);
		EXPECT_STREQ("abc", string.data());
	}

	{
		bzd::Regexp regexp{"a[0-9]+c"_sv};
		bzd::String<12> string;
		const auto result = regexp.capture("a1239c", string.assigner());
		EXPECT_RESULT_EQ(result, 6u);
		EXPECT_STREQ("a1239c", string.data());
	}

	{
		bzd::Regexp regexp{"a[0-9]+c"_sv};
		bzd::String<12> string;
		const auto result = regexp.capture("a1239", string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("a1239", string.data());
	}
	// String too small but match.
	{
		bzd::String<2> string;
		const auto result = bzd::Regexp{"[0-9]+"}.capture("1239", string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("12", string.data());
	}
	// String too small no match.
	{
		bzd::String<4> string;
		const auto result = bzd::Regexp{"[0-9]+a"}.capture("1239", string.assigner());
		EXPECT_FALSE(result);
		EXPECT_STREQ("1239", string.data());
	}
}
