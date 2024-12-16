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
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		using Metadata = typename bzd::FromString<decltype(wrapper)>::Metadata;
		const auto result = bzd::fromString(exactMatch, wrapper, Metadata{Metadata::Mode::greedy});
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), bzd::size(exactMatch));
		EXPECT_STREQ(wrapper.output.value()->data(), exactMatch.data());
	}

	// no match.
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString("false"_sv, wrapper);
		EXPECT_FALSE(result);
	}

	// match a longer string.
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString("infotabil"_sv, wrapper);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 4u);
		EXPECT_STREQ(wrapper.output.value()->data(), "info");
	}

	// lazy.
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		using Metadata = typename bzd::FromString<decltype(wrapper)>::Metadata;
		const auto result = bzd::fromString("hello"_sv, wrapper, Metadata{Metadata::Mode::lazy});
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1u);
		EXPECT_STREQ(wrapper.output.value()->data(), "h");
	}

	// greedy.
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		using Metadata = typename bzd::FromString<decltype(wrapper)>::Metadata;
		const auto result = bzd::fromString("hello"_sv, wrapper, Metadata{Metadata::Mode::greedy});
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 5u);
		EXPECT_STREQ(wrapper.output.value()->data(), "hello");
	}

	// re-using wrapper.
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto result1 = bzd::fromString("hello"_sv, wrapper);
		EXPECT_TRUE(result1);
		EXPECT_EQ(result1.value(), 5u);
		const auto result2 = bzd::fromString("infobar"_sv, wrapper);
		EXPECT_TRUE(result2);
		EXPECT_EQ(result2.value(), 7u);
		const auto result3 = bzd::fromString("notpreset"_sv, wrapper);
		EXPECT_FALSE(result3);
	}
}

TEST(RangeOfRangesFromString, CornerCases)
{
	const auto keywords = {""_sv};

	// empty string is not tested.
	{
		bzd::ToSortedRangeOfRanges wrapper{keywords};
		const auto result = bzd::fromString("hello"_sv, wrapper);
		EXPECT_FALSE(result);
	}

	// empty keywords.
	{
		bzd::ToSortedRangeOfRanges wrapper{bzd::ranges::SubRange{bzd::end(keywords), bzd::end(keywords)}};
		const auto result = bzd::fromString("hello"_sv, wrapper);
		EXPECT_FALSE(result);
	}
}
