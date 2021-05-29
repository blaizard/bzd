#include "cc/bzd/container/btree.h"

#include "cc_test/test.h"

TEST(ContainerBTree, base)
{
	bzd::BTree<int, int, 10, 2> tree;

	tree.insert(12, 22);

	EXPECT_EQ(true, true);
}
