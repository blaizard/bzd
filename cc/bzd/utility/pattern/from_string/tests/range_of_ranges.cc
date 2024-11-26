#include "cc/bzd/utility/pattern/from_string/range_of_ranges.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/range.hh"

TEST(RangeOfRangesFromString, SortedRange)
{
	const auto keywords = {
		"h"_sv,
		"hello"_sv,
		"info"_sv,
		"infobar"_sv,
		"information"_sv,
		"noooo"_sv,
	};

	// Exact matches.
	for (const auto& exactMatch : keywords)
	{
		bzd::ToRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString(exactMatch, wrapper);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), bzd::size(exactMatch));
	}

	// no match.
	{
		bzd::ToRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString("false"_sv, wrapper);
		EXPECT_FALSE(result);
	}

	// match a longer string.
	{
		bzd::ToRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString("infotabil"_sv, wrapper);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
	}
}

TEST(RangeOfRangesFromString, CornerCases)
{
	const auto keywords = {""_sv};

	// empty string is not tested.
	{
		bzd::ToRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString("hello"_sv, wrapper);
		EXPECT_FALSE(result);
	}

	// empty keywords.
	{
		bzd::ToRangeOfRanges wrapper{bzd::ranges::SubRange{bzd::end(keywords), bzd::end(keywords)}};
		const auto result = bzd::fromString("hello"_sv, wrapper);
		EXPECT_FALSE(result);
	}
}
