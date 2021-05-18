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
	bzd::Span<int> spanArray(test);

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

TEST(ContainerSpan, ConstConstIterator)
{
	int test[5] = {0, 1, 2, 3, 4};
	const bzd::Span<const int> span(test, 5);

	{
		auto it = span.begin();
		EXPECT_EQ(*it, 0);
		EXPECT_EQ(*++it, 1);
		EXPECT_EQ(*++it, 2);
		EXPECT_EQ(*++it, 3);
		EXPECT_EQ(*++it, 4);
		EXPECT_EQ(++it, span.end());
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
}

TEST(ContainerSpan, Copy)
{
	int test[5] = {0, 1, 2, 3, 4};
	bzd::Span<int> span(test, 5);

	// Copy constructor
	bzd::Span<int> copySpan(span);
	EXPECT_EQ(copySpan[0], 0);
	EXPECT_EQ(copySpan[4], 4);
	EXPECT_EQ(copySpan.size(), 5);

	// Assignment
	bzd::Span<int> copySpanAssign;
	copySpanAssign = span;
	EXPECT_EQ(copySpanAssign[0], 0);
	EXPECT_EQ(copySpanAssign[4], 4);
	EXPECT_EQ(copySpanAssign.size(), 5);

	// From non-const to const constructor
	bzd::Span<const int> copyConstSpan(span);
	EXPECT_EQ(copyConstSpan[0], 0);
	EXPECT_EQ(copyConstSpan[4], 4);
	EXPECT_EQ(copyConstSpan.size(), 5);

	// From non-const to const Assignment
	bzd::Span<const int> copyConstSpanAssign;
	copyConstSpanAssign = span;
	EXPECT_EQ(copyConstSpanAssign[0], 0);
	EXPECT_EQ(copyConstSpanAssign[4], 4);
	EXPECT_EQ(copyConstSpanAssign.size(), 5);
}

TEST(ContainerSpan, ConstNonConst)
{
	// Both the span and the content can be modified.
	{
		int test[5] = {0, 1, 2, 3, 4};
		bzd::Span<int> spanNonNon{test, 5};
		spanNonNon[1] = 45;
		EXPECT_EQ(spanNonNon[1], 45);
	}
	{
		int test[5] = {0, 1, 2, 3, 4};
		const bzd::Span<int> spanConstNon{test, 5};
		spanConstNon[1] = 45;
		EXPECT_EQ(spanConstNon[1], 45);
	}
	{
		int test[5] = {0, 1, 2, 3, 4};
		bzd::Span<const int> spanNonConst{test, 5};
		// Not possible
		// spanNonConst[1] = 45;
		EXPECT_EQ(spanNonConst[1], 1);
	}
	{
		int test[5] = {0, 1, 2, 3, 4};
		const bzd::Span<const int> spanConstConst{test, 5};
		// Not possible
		// spanConstConst[1] = 45;
		EXPECT_EQ(spanConstConst[1], 1);
	}
}

TEST(ContainerSpan, Subspan)
{
	int test[5] = {0, 1, 2, 3, 4};
	bzd::Span<int> span(test, 5);

	{
		const auto subSpan = span.subSpan();
		EXPECT_EQ(subSpan.size(), 5);
	}
	{
		const auto subSpan = span.subSpan(1);
		EXPECT_EQ(subSpan.size(), 4);
	}
	{
		const auto subSpan = span.subSpan(4);
		EXPECT_EQ(subSpan.size(), 1);
		EXPECT_EQ(subSpan[0], 4);
	}
	{
		const auto subSpan = span.subSpan(1, 2);
		EXPECT_EQ(subSpan.size(), 2);
		EXPECT_EQ(subSpan[0], 1);
		EXPECT_EQ(subSpan[1], 2);
	}
	{
		const auto subSpan = span.subSpan(1, 4);
		EXPECT_EQ(subSpan.size(), 4);
		EXPECT_EQ(subSpan[2], 3);
		EXPECT_EQ(subSpan[3], 4);
	}
	{
		const auto subspan = span.first(2);
		EXPECT_EQ(subspan.size(), 2);
		EXPECT_EQ(subspan[0], 0);
		EXPECT_EQ(subspan[1], 1);
	}
	{
		const auto subSpan = span.last(2);
		EXPECT_EQ(subSpan.size(), 2);
		EXPECT_EQ(subSpan[0], 3);
		EXPECT_EQ(subSpan[1], 4);
	}
}

TEST(ContainerSpan, AsBytes)
{
	bzd::UInt32Type test[5] = {0, 1, 2, 3, 4};
	bzd::Span<bzd::UInt32Type> span(test, 5);

	EXPECT_EQ(span.size(), 5);
	EXPECT_EQ(span.sizeBytes(), 20);

	const auto buffer = span.asBytes();
	EXPECT_EQ(buffer.size(), 20);
	EXPECT_EQ(buffer.sizeBytes(), 20);

	const auto writableBuffer = span.asWritableBytes();
	EXPECT_EQ(writableBuffer.size(), 20);
	EXPECT_EQ(writableBuffer.sizeBytes(), 20);
}
