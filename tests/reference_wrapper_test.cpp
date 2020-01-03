#include "bzd/utility/reference_wrapper.h"

#include "gtest/gtest.h"

TEST(ReferenceWrapper, simple)
{
	int a = 42;
	bzd::ReferenceWrapper<int> refA = a;

	EXPECT_EQ(refA, 42);
	EXPECT_EQ(refA, a);

	refA.get() = -12;

	EXPECT_EQ(a, -12);
}
