#include "cc/bzd/container/queue.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerForwardList, simple)
{
	bzd::Queue<int, 5> queue;

	queue.push(0);
	queue.push(1);

	EXPECT_EQ(queue.front(), 0);
	EXPECT_EQ(queue.back(), 1);

	queue.push(2);

	EXPECT_EQ(queue.front(), 0);
	EXPECT_EQ(queue.back(), 2);

	queue.pop();

	EXPECT_EQ(queue.front(), 1);
	EXPECT_EQ(queue.back(), 2);

	queue.pop();

	EXPECT_EQ(queue.front(), 2);
	EXPECT_EQ(queue.back(), 2);

	queue.pop();
	queue.push(3);

	EXPECT_EQ(queue.front(), 3);
	EXPECT_EQ(queue.back(), 3);
}
