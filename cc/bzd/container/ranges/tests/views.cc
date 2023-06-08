#include "cc/bzd/container/ranges/views/views.hh"

#include "cc/bzd/algorithm/equal.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/move_only.hh"
#include "cc/bzd/test/types/range.hh"
/*
TEST(Views, Borrowed)
{
	const auto expected = {2, 3, 4};
	// This should fail.
	bzd::ignore = bzd::algorithm::equal(bzd::makeArray(0, 1, 2, 3) | bzd::ranges::drop(2), expected);
}
*/

template <class Range, class Expected>
void helperTestCopy(Range&& range, Expected&& expected)
{
	// Copy constructor.
	auto copy{range};
	{
		const auto isEqual = bzd::algorithm::equal(copy, expected);
		EXPECT_TRUE(isEqual);
	}
	// Copy assignment.
	copy = range;
	{
		const auto isEqual = bzd::algorithm::equal(copy, expected);
		EXPECT_TRUE(isEqual);
	}
}

template <class Range, class Expected>
void helperTestMove(Range&& range, Expected&& expected)
{
	// Move constructor.
	auto move{bzd::move(range)};
	{
		const auto isEqual = bzd::algorithm::equal(move, expected);
		EXPECT_TRUE(isEqual);
	}
	// Move assignment.
	move = bzd::move(range);
	{
		const auto isEqual = bzd::algorithm::equal(move, expected);
		EXPECT_TRUE(isEqual);
	}
}

TEST(Views, Drop)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Drop view{bzd::inPlace, range, 2};

	const auto expected = {2, 3, 4};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto isEqual = bzd::algorithm::equal(bzd::move(view) | bzd::ranges::all(), expected);
		// This should not compile.
		// const auto isEqual = bzd::algorithm::equal(bzd::move(range) | bzd::ranges::drop(2), expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto isEqual = bzd::algorithm::equal(range | bzd::ranges::drop(2), expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto expected2 = {3, 4};
		const auto isEqual = bzd::algorithm::equal(range | bzd::ranges::drop(2) | bzd::ranges::drop(1), expected2);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto isEqual = bzd::algorithm::equal("hello"_sv | bzd::ranges::drop(2), "llo"_sv);
		EXPECT_TRUE(isEqual);
	}

	EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::ranges::Drop<bzd::Span<int>>>);
	EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::ranges::Drop<bzd::String<1u>&>>);

	helperTestCopy(bzd::ranges::Drop{bzd::inPlace, "012345"_sv, 2}, "2345"_sv);
	helperTestMove(bzd::ranges::Drop{bzd::inPlace, "012345"_sv, 2}, "2345"_sv);
}

TEST(Views, Take)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Take view{bzd::inPlace, range, 2};

	const auto expected = {0, 1};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto isEqual = bzd::algorithm::equal(range | bzd::ranges::take(2), expected);
		EXPECT_TRUE(isEqual);
	}
	{
		const auto expected2 = {0};
		const auto isEqual = bzd::algorithm::equal(range | bzd::ranges::take(2) | bzd::ranges::take(1), expected2);
		EXPECT_TRUE(isEqual);
	}

	helperTestCopy(bzd::ranges::Take{bzd::inPlace, "012345"_sv, 2}, "01"_sv);
	helperTestMove(bzd::ranges::Take{bzd::inPlace, "012345"_sv, 2}, "01"_sv);
}

TEST(Views, Transform)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Transform view{bzd::inPlace, range, [](const auto value) { return value * value; }};

	const auto expected = {0, 1, 4, 9, 16};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}

	helperTestCopy(bzd::ranges::Transform{bzd::inPlace, "ABC"_sv, [](const char c) { return c + ('a' - 'A'); }}, "abc"_sv);
	helperTestMove(bzd::ranges::Transform{bzd::inPlace, "ABC"_sv, [](const char c) { return c + ('a' - 'A'); }}, "abc"_sv);
}

TEST(Views, Reverse)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Reverse view{bzd::inPlace, range};

	const auto expected = {4, 3, 2, 1, 0};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}

	helperTestCopy(bzd::ranges::Reverse{bzd::inPlace, "012345"_sv}, "543210"_sv);
	helperTestMove(bzd::ranges::Reverse{bzd::inPlace, "012345"_sv}, "543210"_sv);
}

TEST(Views, Owning)
{
	bzd::StringView v{"Hello World"_sv};
	auto myOwningView = bzd::ranges::Owning(bzd::inPlace, std::move(v));
	EXPECT_EQ(myOwningView.size(), 11);
	{
		const auto isEqual = bzd::algorithm::equal(myOwningView, "Hello World"_sv);
		EXPECT_TRUE(isEqual);
	}

	helperTestMove(bzd::ranges::Owning{bzd::inPlace, "012345"_sv}, "012345"_sv);
}
