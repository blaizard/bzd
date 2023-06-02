#include "cc/bzd/container/range/views/views.hh"

#include "cc/bzd/algorithm/equal.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/range.hh"

/*
TEST(Views, Borrowed)
{
	const auto expected = {2, 3, 4};
	// This should fail.
	bzd::ignore = bzd::algorithm::equal(bzd::makeArray(0, 1, 2, 3) | bzd::range::drop(2), expected);
}
*/

TEST(Views, Drop)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Drop view{bzd::inPlace, range, 2};

	const auto expected = {2, 3, 4};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto isEqual = bzd::algorithm::equal(range | bzd::range::drop(2), expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto expected2 = {3, 4};
		const auto isEqual = bzd::algorithm::equal(range | bzd::range::drop(2) | bzd::range::drop(1), expected2);
		EXPECT_TRUE(isEqual);
	}

	EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::range::Drop<bzd::Span<int>>>);
	// EXPECT_FALSE(bzd::concepts::borrowedRange<bzd::range::Drop<bzd::String<1u>>>);
	EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::range::Drop<bzd::String<1u>&>>);
	// EXPECT_FALSE(bzd::concepts::borrowedRange<bzd::range::Drop<bzd::String<1u>&&>>);
}

TEST(Views, Take)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Take view{bzd::inPlace, range, 2};

	const auto expected = {0, 1};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto isEqual = bzd::algorithm::equal(range | bzd::range::take(2), expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto expected2 = {0};
		const auto isEqual = bzd::algorithm::equal(range | bzd::range::take(2) | bzd::range::take(1), expected2);
		EXPECT_TRUE(isEqual);
	}

	/*
		EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::range::Take<bzd::Span<int>>>);
		EXPECT_FALSE(bzd::concepts::borrowedRange<bzd::range::Take<bzd::String<1u>>>);
		EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::range::Take<bzd::String<1u>&>>);
		EXPECT_FALSE(bzd::concepts::borrowedRange<bzd::range::Take<bzd::String<1u>&&>>);
	*/
}

TEST(Views, Transform)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Transform view{bzd::inPlace, range, [](const auto value) { return value * value; }};

	const auto expected = {0, 1, 4, 9, 16};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
}

TEST(Views, Reverse)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Reverse view{bzd::inPlace, range};

	const auto expected = {4, 3, 2, 1, 0};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
}
