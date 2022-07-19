#include "cc/bzd/container/pool.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerPool, single)
{
	bzd::Pool<int, 1> pool;
	EXPECT_EQ(pool.capacity(), 1U);

	{
		auto& item = pool.reserve();
		EXPECT_TRUE(pool.empty());

		item = 42;
		EXPECT_EQ(item, 42);

		pool.release(item);
		EXPECT_FALSE(pool.empty());
	}

	{
		auto& item = pool.reserve();
		EXPECT_TRUE(pool.empty());

		EXPECT_EQ(item, 42);
	}
}

TEST(ContainerPool, multi)
{
	bzd::Pool<int, 3> pool;
	EXPECT_EQ(pool.capacity(), 3U);

	auto& item1 = pool.reserve();
	EXPECT_FALSE(pool.empty());
	auto& item2 = pool.reserve();
	EXPECT_FALSE(pool.empty());
	auto& item3 = pool.reserve();
	EXPECT_TRUE(pool.empty());

	pool.release(item1);
	EXPECT_FALSE(pool.empty());

	auto& item4 = pool.reserve();
	EXPECT_TRUE(pool.empty());

	pool.release(item2);
	pool.release(item3);
	pool.release(item4);
	EXPECT_FALSE(pool.empty());
}
