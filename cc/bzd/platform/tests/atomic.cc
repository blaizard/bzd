#include "cc/bzd/platform/atomic.h"

#include "cc/bzd/platform/types.h"
#include "cc_test/test.h"

template <class T>
void baseTypeUnitTest()
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

TEST(Atomic, Base)
{
	baseTypeUnitTest<bzd::UInt8Type>();
	baseTypeUnitTest<bzd::Int8Type>();
	baseTypeUnitTest<bzd::UInt16Type>();
	baseTypeUnitTest<bzd::Int16Type>();
	baseTypeUnitTest<bzd::UInt32Type>();
	baseTypeUnitTest<bzd::Int32Type>();
}
