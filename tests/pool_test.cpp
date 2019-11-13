#include "gtest/gtest.h"
#include "bzd/container/pool.h"

TEST(ContainerPool, base)
{
	bzd::Pool<int, 5> pool;

	pool.toStream(std::cout);
	std::cout << std::endl;

	auto item = pool.reserve();
	auto item2 = pool.reserve();
	auto item3 = pool.reserve();

	pool.toStream(std::cout);
	std::cout << std::endl;

	pool.release(item2);

	pool.toStream(std::cout);
	std::cout << std::endl;

    EXPECT_EQ(true, true);
}
