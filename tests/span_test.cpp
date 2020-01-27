#include "bzd/container/span.h"

#include "cc_test/test.h"

TEST(ContainerSpan, Base)
{
	int test[5];
	bzd::Span<int> span(test, 5);

	EXPECT_EQ(span.size(), 5);
}

TEST(ContainerSpan, Constructor)
{
	int test[5];
	bzd::Span<int> span(test, 5);

	// Copy
	bzd::Span<int> spanCopy(span);
	EXPECT_EQ(spanCopy.size(), 5);

	// Copy non-const to const
	bzd::Span<const int> spanCopyConst(span);
	EXPECT_EQ(spanCopyConst.size(), 5);

	// Copy const to const
	bzd::Span<const int> spanCopyConstSquare(spanCopyConst);
	EXPECT_EQ(spanCopyConstSquare.size(), 5);

	// Copy const to non-const (not allowed!)
	// bzd::Span<int> spanCopyNonConst(spanCopyConst);
}

TEST(ContainerSpan, Iterator)
{
	int test[5] = {0, 1, 2, 3, 4};
	bzd::Span<int> span(test, 5);

	{
		auto it = span.begin();
		EXPECT_EQ(*it, 0);
		EXPECT_EQ(*++it, 1);
		EXPECT_EQ(*++it, 2);
		EXPECT_EQ(*++it, 3);
		EXPECT_EQ(*++it, 4);
		EXPECT_EQ(++it, span.end());
	}

	for (auto& c : span)
	{
		c = 'A';
	}
	EXPECT_EQ(span[0], 'A');
	EXPECT_EQ(span[1], 'A');
	EXPECT_EQ(span[2], 'A');
	EXPECT_EQ(span[3], 'A');
	EXPECT_EQ(span[4], 'A');
}

TEST(ContainerSpan, ConstIterator)
{
	int test[5] = {0, 1, 2, 3, 4};
	const bzd::Span<int> span(test, 5);

	{
		auto it = span.begin();
		EXPECT_EQ(*it, 0);
		EXPECT_EQ(*++it, 1);
		EXPECT_EQ(*++it, 2);
		EXPECT_EQ(*++it, 3);
		EXPECT_EQ(*++it, 4);
		EXPECT_EQ(++it, span.end());
	}

	{
		auto it = span.cbegin();
		EXPECT_EQ(*it, 0);
		EXPECT_EQ(*++it, 1);
		EXPECT_EQ(*++it, 2);
		EXPECT_EQ(*++it, 3);
		EXPECT_EQ(*++it, 4);
		EXPECT_EQ(++it, span.cend());
	}
}

TEST(ContainerSpan, Constexpr)
{
	static constexpr int test[5] = {0, 1, 2, 3, 4};
	constexpr bzd::Span<const int> span(test, 5);

	{
		auto it = span.begin();
		EXPECT_EQ(*it, 0);
		EXPECT_EQ(*++it, 1);
		EXPECT_EQ(*++it, 2);
		EXPECT_EQ(*++it, 3);
		EXPECT_EQ(*++it, 4);
		EXPECT_EQ(++it, span.end());
	}

	{
		auto it = span.cbegin();
		EXPECT_EQ(*it, 0);
		EXPECT_EQ(*++it, 1);
		EXPECT_EQ(*++it, 2);
		EXPECT_EQ(*++it, 3);
		EXPECT_EQ(*++it, 4);
		EXPECT_EQ(++it, span.cend());
	}
}
