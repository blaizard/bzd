#include "cc/bzd/platform/atomic.hh"

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/test/test.hh"

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
	baseTypeUnitTest<bzd::UInt8>();
	baseTypeUnitTest<bzd::Int8>();
	baseTypeUnitTest<bzd::UInt16>();
	baseTypeUnitTest<bzd::Int16>();
	baseTypeUnitTest<bzd::UInt32>();
	baseTypeUnitTest<bzd::Int32>();
}
