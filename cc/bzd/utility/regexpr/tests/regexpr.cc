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
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"abc"_sv}), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"Hello"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"qabc"_sv}));
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"abcq"_sv}), 3u);
}

TEST(Regexpr, SpecialChar)
{
	EXPECT_RESULT_EQ(bzd::Regexpr{"a\\*c"}.match(bzd::range::Stream{"a*c"_sv}), 3u);
	EXPECT_RESULT_EQ(bzd::Regexpr{"a.c"}.match(bzd::range::Stream{"abc"_sv}), 3u);
	EXPECT_FALSE(bzd::Regexpr{"\\"}.match(bzd::range::Stream{""_sv}));
	EXPECT_TRUE(bzd::Regexpr{"\\\\"}.match(bzd::range::Stream{"\\"_sv}));
	EXPECT_TRUE(bzd::Regexpr{"\\s"}.match(bzd::range::Stream{" "_sv}));
	EXPECT_TRUE(bzd::Regexpr{"\\s+a"}.match(bzd::range::Stream{" \t\na"_sv}));
}

TEST(Regexpr, Brackets)
{
	bzd::Regexpr regexpr{"a[bcde]f"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"abf"_sv}), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"adf"_sv}), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"adef"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"ad"_sv}));
}

TEST(Regexpr, BracketsRange)
{
	bzd::Regexpr regexpr{"a[b-e]f"};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"abf"_sv}), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"adf"_sv}), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"agf"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"adef"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"ad"_sv}));
	EXPECT_FALSE(bzd::Regexpr{"a[b"}.match(bzd::range::Stream{"ab"_sv}));
	EXPECT_FALSE(bzd::Regexpr{"a["}.match(bzd::range::Stream{"a"_sv}));
	EXPECT_FALSE(bzd::Regexpr{"a[]"}.match(bzd::range::Stream{"a"_sv}));
}

TEST(Regexpr, BracketsNegate)
{
	bzd::Regexpr regexpr{"a[^bd]f"};
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"abf"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"adf"_sv}));
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"agf"_sv}), 3u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"adef"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"ad"_sv}));
	EXPECT_FALSE(bzd::Regexpr{"a[b"}.match(bzd::range::Stream{"ab"_sv}));
	EXPECT_FALSE(bzd::Regexpr{"a["}.match(bzd::range::Stream{"a"_sv}));
	EXPECT_FALSE(bzd::Regexpr{"a[]"}.match(bzd::range::Stream{"a"_sv}));
}

TEST(Regexpr, Star)
{
	bzd::Regexpr regexpr{"a*c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"aaaaac"_sv}), 6u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"c"_sv}), 1u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"acb"_sv}), 2u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"cb"_sv}), 1u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"bc"_sv}));
	EXPECT_RESULT_EQ(bzd::Regexpr{"a*b*"}.match(bzd::range::Stream{"ab"_sv}), 2u);
	EXPECT_FALSE(bzd::Regexpr{"*"}.match(bzd::range::Stream{""_sv}));
	EXPECT_FALSE(bzd::Regexpr{"*"}.match(bzd::range::Stream{"*"_sv}));
}

TEST(Regexpr, StarBrackets)
{
	bzd::Regexpr regexpr{"a[0-9]*c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"a1c"_sv}), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"a0123456789c"_sv}), 12u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"ac"_sv}), 2u);
}

TEST(Regexpr, Plus)
{
	bzd::Regexpr regexpr{"a+c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"aaaaac"_sv}), 6u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"c"_sv}));
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"acb"_sv}), 2u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"cb"_sv}));
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"bc"_sv}));
	EXPECT_RESULT_EQ(bzd::Regexpr{"a+b+"}.match(bzd::range::Stream{"ab"_sv}), 2u);
	EXPECT_FALSE(bzd::Regexpr{"+"}.match(bzd::range::Stream{""_sv}));
	EXPECT_FALSE(bzd::Regexpr{"+"}.match(bzd::range::Stream{"+"_sv}));
}

TEST(Regexpr, PlusBrackets)
{
	bzd::Regexpr regexpr{"a[0-9]+c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"a1c"_sv}), 3u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"a0123456789c"_sv}), 12u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"ac"_sv}));
}

TEST(Regexpr, QuestionMark)
{
	bzd::Regexpr regexpr{"a?c"_sv};
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"ac"_sv}), 2u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"c"_sv}), 1u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"acb"_sv}), 2u);
	EXPECT_RESULT_EQ(regexpr.match(bzd::range::Stream{"cb"_sv}), 1u);
	EXPECT_FALSE(regexpr.match(bzd::range::Stream{"bc"_sv}));
	EXPECT_RESULT_EQ(bzd::Regexpr{"a?b?"}.match(bzd::range::Stream{"ab"_sv}), 2u);
	EXPECT_FALSE(bzd::Regexpr{"?"}.match(bzd::range::Stream{""_sv}));
	EXPECT_FALSE(bzd::Regexpr{"?"}.match(bzd::range::Stream{"?"_sv}));
}