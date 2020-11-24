#include "bzd/platform/atomic.h"

#include "cc_test/test.h"

TEST(Atomic, Base)
{
	bzd::Atomic<int> empty{};
	(void)empty;

	bzd::Atomic<int> test{12};
	EXPECT_EQ(test.load(), 12);

	test += 2;
	EXPECT_EQ(test.load(), 14);
	test -= 2;
	EXPECT_EQ(test.load(), 12);
	test |= 0x10;
	EXPECT_EQ(test.load(), 28);
	test &= 0xf;
	EXPECT_EQ(test.load(), 12);
	test ^= 0xa;
	EXPECT_EQ(test.load(), 6);
	++test;
	EXPECT_EQ(test.load(), 7);
	--test;
	EXPECT_EQ(test.load(), 6);
}
