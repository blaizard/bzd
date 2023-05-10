#include "cc/bzd/container/range/views/views.hh"

#include "cc/bzd/algorithm/equal.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/range.hh"

TEST(Views, Drop)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Drop view{range, 2};

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
}

TEST(Views, Take)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Take view{range, 2};

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
}

TEST(Views, Transform)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::forward, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Transform view{range, [](const auto value) { return value * value; }};

	const auto expected = {0, 1, 4, 9, 16};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
}

TEST(Views, Reverse)
{
	bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Reverse view{range};

	const auto expected = {4, 3, 2, 1, 0};
	{
		const auto isEqual = bzd::algorithm::equal(view, expected);
		EXPECT_TRUE(isEqual);
	}
}
