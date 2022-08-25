#include "cc/bzd/container/threadsafe/bitset.hh"

#include "cc/bzd/test/test.hh"

TEST(Bitset, Basic)
{
	bzd::threadsafe::Bitset<10> container;
	ASSERT_EQ(container.size(), 10u);
	ASSERT_EQ(container.countLZero(), 10u);
	ASSERT_EQ(container.countLOne(), 0u);
	ASSERT_TRUE(container.compareSet(0U));
	ASSERT_FALSE(container.compareSet(0U));
	ASSERT_EQ(container, bzd::threadsafe::Bitset{"1000000000"});
}

TEST(Bitset, ConstructorStringSmall)
{
	bzd::threadsafe::Bitset container{"01001"};
	ASSERT_EQ(container.size(), 5u);
	ASSERT_EQ(container.countLZero(), 1u);
	ASSERT_EQ(container.countLOne(), 0u);
	ASSERT_TRUE(container.compareSet(3U));
	ASSERT_EQ(container, bzd::threadsafe::Bitset{"01011"});
}

TEST(Bitset, ConstructorStringLarge)
{
	bzd::threadsafe::Bitset container{"0100100001010011110000110111001010101010100100011110100101"};
	ASSERT_EQ(container.size(), 58u);
	ASSERT_EQ(container.countLZero(), 1u);
	ASSERT_EQ(container.countLOne(), 0u);
}
