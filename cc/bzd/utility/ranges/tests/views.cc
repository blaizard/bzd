#include "cc/bzd/utility/ranges/views/views.hh"

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
	EXPECT_EQ_RANGE(copy, expected);

	// Copy assignment.
	copy = range;
	EXPECT_EQ_RANGE(copy, expected);
}

template <class Range, class Expected>
void helperTestMove(Range&& range, Expected&& expected)
{
	// Move constructor.
	auto move{bzd::move(range)};
	EXPECT_EQ_RANGE(move, expected);

	// Move assignment.
	move = bzd::move(range);
	EXPECT_EQ_RANGE(move, expected);
}

TEST(Views, Drop)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Drop view{bzd::inPlace, range, 2};

	const auto expected = {2, 3, 4};
	EXPECT_EQ_RANGE(view, expected);
	EXPECT_EQ_RANGE(bzd::move(view) | bzd::ranges::all(), expected);
	EXPECT_EQ_RANGE(range | bzd::ranges::drop(2), expected);

	const auto expected2 = {3, 4};
	EXPECT_EQ_RANGE(range | bzd::ranges::drop(2) | bzd::ranges::drop(1), expected2);

	EXPECT_EQ_RANGE("hello"_sv | bzd::ranges::drop(2), "llo"_sv);

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
	EXPECT_EQ_RANGE(view, expected);
	EXPECT_EQ_RANGE(range | bzd::ranges::take(2), expected);

	const auto expected2 = {0};
	EXPECT_EQ_RANGE(range | bzd::ranges::take(2) | bzd::ranges::take(1), expected2);

	helperTestCopy(bzd::ranges::Take{bzd::inPlace, "012345"_sv, 2}, "01"_sv);
	helperTestMove(bzd::ranges::Take{bzd::inPlace, "012345"_sv, 2}, "01"_sv);
}

TEST(Views, Transform)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Transform view{bzd::inPlace, range, [](const auto value) { return value * value; }};

	const auto expected = {0, 1, 4, 9, 16};
	EXPECT_EQ_RANGE(view, expected);

	helperTestCopy(bzd::ranges::Transform{bzd::inPlace, "ABC"_sv, [](const char c) { return c + ('a' - 'A'); }}, "abc"_sv);
	helperTestMove(bzd::ranges::Transform{bzd::inPlace, "ABC"_sv, [](const char c) { return c + ('a' - 'A'); }}, "abc"_sv);
}

TEST(Views, Reverse)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional, int, 5> range{0, 1, 2, 3, 4};
	bzd::ranges::Reverse view{bzd::inPlace, range};

	const auto expected = {4, 3, 2, 1, 0};
	EXPECT_EQ_RANGE(view, expected);

	helperTestCopy(bzd::ranges::Reverse{bzd::inPlace, "012345"_sv}, "543210"_sv);
	helperTestMove(bzd::ranges::Reverse{bzd::inPlace, "012345"_sv}, "543210"_sv);
}

TEST(Views, Owning)
{
	bzd::StringView v{"Hello World"_sv};
	auto myOwningView = bzd::ranges::Owning(bzd::inPlace, std::move(v));
	EXPECT_EQ(myOwningView.size(), 11);

	EXPECT_EQ_RANGE(myOwningView, "Hello World"_sv);

	helperTestMove(bzd::ranges::Owning{bzd::inPlace, "012345"_sv}, "012345"_sv);
}

TEST(Views, Join)
{
	const auto container = {"Hello"_sv, " "_sv, "World"_sv};
	auto view = bzd::ranges::Join(bzd::inPlace, container);
	EXPECT_EQ(view.size(), 11u);

	EXPECT_EQ_RANGE(view, "Hello World"_sv);

	helperTestCopy(bzd::ranges::Join{bzd::inPlace, container}, "Hello World"_sv);
	helperTestMove(bzd::ranges::Join{bzd::inPlace, container}, "Hello World"_sv);
}

TEST(Views, Filter)
{
	const auto container = {2, 3, -3, 1, 0, -20, -1, 2};
	const auto predicate = [](const auto value) { return value > 0; };
	bzd::ranges::Filter view{bzd::inPlace, container, predicate};

	const auto expected = {2, 3, 1, 2};
	EXPECT_EQ_RANGE(view, expected);

	helperTestCopy(bzd::ranges::Filter{bzd::inPlace, container, predicate}, expected);
	helperTestMove(bzd::ranges::Filter{bzd::inPlace, container, predicate}, expected);
}
