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
void helperTestMove(Range&& range1, Range&& range2, Expected&& expected)
{
	// Move constructor.
	auto move{bzd::move(range1)};
	EXPECT_EQ_RANGE(move, expected);

	// Move assignment.
	move = bzd::move(range2);
	EXPECT_EQ_RANGE(move, expected);
}

TEST(Views, All)
{
	bzd::Array<int, 100> data{};
	EXPECT_FALSE(bzd::concepts::view<decltype(data)>);

	// This view is a reference of the data.
	auto viewRef = data | bzd::ranges::all();
	static_assert(bzd::typeTraits::isSame<decltype(viewRef), bzd::ranges::Ref<bzd::Array<int, 100>>>);
	EXPECT_LT(sizeof(viewRef), 100u);
	EXPECT_TRUE(bzd::concepts::view<decltype(viewRef)>);
	EXPECT_TRUE(bzd::concepts::borrowedRange<decltype(viewRef)>);

	// This view owns the data.
	auto viewOwning = bzd::move(data) | bzd::ranges::all();
	static_assert(bzd::typeTraits::isSame<decltype(viewOwning), bzd::ranges::Owning<bzd::Array<int, 100>>>);
	EXPECT_GT(sizeof(viewOwning), 100u);
	EXPECT_TRUE(bzd::concepts::view<decltype(viewOwning)>);
	EXPECT_FALSE(bzd::concepts::borrowedRange<decltype(viewOwning)>);

	// This view::all of a view::all is the same view.
	auto viewRefNested = viewRef | bzd::ranges::all() | bzd::ranges::all() | bzd::ranges::all();
	static_assert(bzd::typeTraits::isSame<decltype(viewRefNested), bzd::ranges::Ref<bzd::Array<int, 100>>>);
	EXPECT_LT(sizeof(viewRefNested), 100u);
	EXPECT_TRUE(bzd::concepts::view<decltype(viewRefNested)>);
	EXPECT_TRUE(bzd::concepts::borrowedRange<decltype(viewRefNested)>);

	// This view::all of a view::all is the same view.
	auto viewOwningNested = viewOwning | bzd::ranges::all() | bzd::ranges::all() | bzd::ranges::all();
	static_assert(bzd::typeTraits::isSame<decltype(viewOwningNested), bzd::ranges::Owning<bzd::Array<int, 100>>>);
	EXPECT_GT(sizeof(viewOwningNested), 100u);
	EXPECT_TRUE(bzd::concepts::view<decltype(viewOwningNested)>);
	EXPECT_FALSE(bzd::concepts::borrowedRange<decltype(viewOwningNested)>);

	helperTestCopy(viewRefNested, data);
	auto viewOwning2 = bzd::Array<int, 100>{} | bzd::ranges::all();
	helperTestMove(bzd::move(viewOwningNested), bzd::move(viewOwning2), data);
}

TEST(Views, Owning)
{
	bzd::StringView v{"Hello World"_sv};
	auto myOwningView = bzd::ranges::Owning(std::move(v));
	EXPECT_EQ(myOwningView.size(), 11);

	EXPECT_EQ_RANGE(myOwningView, "Hello World"_sv);

	helperTestMove(bzd::ranges::Owning{"012345"_sv}, bzd::ranges::Owning{"012345"_sv}, "012345"_sv);
}

TEST(Views, Drop)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	auto view = range | bzd::ranges::drop(2);
	static_assert(bzd::typeTraits::isSame<decltype(view), bzd::ranges::Drop<bzd::ranges::Ref<decltype(range)>>>);

	const auto expected = {2, 3, 4};
	EXPECT_EQ_RANGE(view, expected);
	EXPECT_EQ_RANGE(bzd::move(view) | bzd::ranges::all(), expected);
	EXPECT_EQ_RANGE(range | bzd::ranges::drop(2), expected);

	const auto expected2 = {3, 4};
	EXPECT_EQ_RANGE(range | bzd::ranges::drop(2) | bzd::ranges::drop(1), expected2);

	EXPECT_EQ_RANGE("hello"_sv | bzd::ranges::drop(2), "llo"_sv);

	// EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::ranges::Drop<bzd::Span<int>>>);
	// EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::ranges::Drop<bzd::String<1u>&>>);

	const auto data = "012345"_sv;
	helperTestCopy(data | bzd::ranges::drop(2), "2345"_sv);
	helperTestMove("012345"_sv | bzd::ranges::drop(2), "012345"_sv | bzd::ranges::drop(2), "2345"_sv);
}

TEST(Views, Take)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	auto view = range | bzd::ranges::take(2);
	static_assert(bzd::typeTraits::isSame<decltype(view), bzd::ranges::Take<bzd::ranges::Ref<decltype(range)>>>);

	const auto expected = {0, 1};
	EXPECT_EQ_RANGE(view, expected);
	EXPECT_EQ_RANGE(range | bzd::ranges::take(2), expected);

	const auto expected2 = {0};
	EXPECT_EQ_RANGE(range | bzd::ranges::take(2) | bzd::ranges::take(1), expected2);

	const auto data = "012345"_sv;
	helperTestCopy(data | bzd::ranges::take(2), "01"_sv);
	helperTestMove("012345"_sv | bzd::ranges::take(2), "012345"_sv | bzd::ranges::take(2), "01"_sv);
}

TEST(Views, DropLast)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional, int, 5> range{0, 1, 2, 3, 4};
	auto view = range | bzd::ranges::dropLast(1);
	static_assert(bzd::typeTraits::isSame<decltype(view), bzd::ranges::DropLast<bzd::ranges::Ref<decltype(range)>>>);

	const auto expected = {0, 1, 2, 3};
	EXPECT_EQ_RANGE(view, expected);
	EXPECT_EQ_RANGE(bzd::move(view) | bzd::ranges::all(), expected);
	EXPECT_EQ_RANGE(range | bzd::ranges::dropLast(1), expected);

	const auto expected2 = {0, 1};
	EXPECT_EQ_RANGE(range | bzd::ranges::dropLast(2) | bzd::ranges::dropLast(1), expected2);

	EXPECT_EQ_RANGE("hello"_sv | bzd::ranges::dropLast(2), "hel"_sv); // codespell:ignore

	// EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::ranges::Drop<bzd::Span<int>>>);
	// EXPECT_TRUE(bzd::concepts::borrowedRange<bzd::ranges::Drop<bzd::String<1u>&>>);

	const auto data = "012345"_sv;
	helperTestCopy(data | bzd::ranges::dropLast(2), "0123"_sv);
	helperTestMove("012345"_sv | bzd::ranges::dropLast(2), "012345"_sv | bzd::ranges::dropLast(2), "0123"_sv);
}

TEST(Views, Transform)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	auto view = range | bzd::ranges::transform([](const auto value) { return value * value; });

	const auto expected = {0, 1, 4, 9, 16};
	EXPECT_EQ_RANGE(view, expected);

	const auto data = "ABC"_sv;
	const auto transformFct = [](const char c) { return c + ('a' - 'A'); };
	helperTestCopy(data | bzd::ranges::transform(transformFct), "abc"_sv);
	helperTestMove("ABC"_sv | bzd::ranges::transform(transformFct), "ABC"_sv | bzd::ranges::transform(transformFct), "abc"_sv);
}

TEST(Views, Reverse)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional, int, 5> range{0, 1, 2, 3, 4};
	auto view = range | bzd::ranges::reverse();
	static_assert(bzd::typeTraits::isSame<decltype(view), bzd::ranges::Reverse<bzd::ranges::Ref<decltype(range)>>>);

	const auto expected1 = {4, 3, 2, 1, 0};
	EXPECT_EQ_RANGE(view, expected1);

	const auto expected2 = {0, 1, 2, 3, 4};
	EXPECT_EQ_RANGE(range | bzd::ranges::reverse() | bzd::ranges::reverse(), expected2);

	const auto data = "012345"_sv;
	helperTestCopy(data | bzd::ranges::reverse(), "543210"_sv);
	helperTestMove("012345"_sv | bzd::ranges::reverse(), "012345"_sv | bzd::ranges::reverse(), "543210"_sv);
}

TEST(Views, Join)
{
	const auto container = {"Hello"_sv, " "_sv, "World"_sv};
	auto view = container | bzd::ranges::join();
	EXPECT_EQ(view.size(), 11u);
	static_assert(bzd::typeTraits::isSame<decltype(view), bzd::ranges::Join<bzd::ranges::Ref<decltype(container)>>>);

	EXPECT_EQ_RANGE(view, "Hello World"_sv);

	helperTestCopy(container | bzd::ranges::join(), "Hello World"_sv);
	helperTestMove(container | bzd::ranges::join(), container | bzd::ranges::join(), "Hello World"_sv);
}

TEST(Views, Filter)
{
	const auto container = {2, 3, -3, 1, 0, -20, -1, 2};
	auto predicate = [](const auto value) { return value > 0; };
	auto view = container | bzd::ranges::filter(predicate);
	static_assert(bzd::typeTraits::isSame<decltype(view), bzd::ranges::Filter<bzd::ranges::Ref<decltype(container)>, decltype(predicate)>>);

	const auto expected = {2, 3, 1, 2};
	EXPECT_EQ_RANGE(view, expected);

	helperTestCopy(container | bzd::ranges::filter(predicate), expected);
	helperTestMove(container | bzd::ranges::filter(predicate), container | bzd::ranges::filter(predicate), expected);
}
