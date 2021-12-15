#include "cc/bzd/container/spans.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerSpans, Base)
{
	int array[] = {0, 1, 2, 3, 4, -1, 5, 6, 7};
	bzd::Spans<int, 2> spans(bzd::inPlace, bzd::Span<int>{array, 5}, bzd::Span<int>{array + 6, 3});

	EXPECT_EQ(spans.size(), 8U);
	EXPECT_EQ(spans.array().size(), 2U);
}

TEST(ContainerSpans, SubSpans)
{
	int array[] = {0, 1, 2, 3, 4, -1, 5, 6, 7};
	bzd::Spans<int, 2> spans(bzd::inPlace, bzd::Span<int>{array, 5}, bzd::Span<int>{array + 6, 3});

	{
		const auto sub = spans.subSpans(3);
		EXPECT_EQ(sub.size(), 5U);
	}

	{
		const auto sub = spans.subSpans(3, 2);
		EXPECT_EQ(sub.size(), 2U);
	}
}
