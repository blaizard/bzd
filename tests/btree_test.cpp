#include "bzd/container/btree.h"

#include "gtest/gtest.h"

TEST(ContainerBTree, base)
{
	bzd::BTree<int, int, 10, 2> tree;

	tree.insert(12, 42);

	EXPECT_EQ(true, true);
}
