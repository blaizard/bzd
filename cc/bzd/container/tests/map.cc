#include "bzd/container/map.h"

#include "cc_test/test.h"

TEST(ContainerMap, single)
{
	bzd::Map<int, int, 12> map;

	map.insert(12, 5);

	{
		const auto result = map.find(42);
		EXPECT_FALSE(result);
	}
	{
		const auto result = map.find(12);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value()->second, 5);
		EXPECT_EQ(map[12], 5);
	}
}
