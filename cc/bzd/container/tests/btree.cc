#include "cc/bzd/container/btree.hh"

#include "cc_test/test.hh"

TEST(ContainerBTree, base)
{
	bzd::BTree<int, int, 10, 2> tree;

	tree.insert(12, 22);

	EXPECT_EQ(true, true);
}
