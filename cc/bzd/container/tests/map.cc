#include "cc/bzd/container/map.hh"

#include "cc/bzd/test/test.hh"

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

TEST_CONSTEXPR_BEGIN(ContainerMap, Constexpr)
{
	bzd::Map<int, int, 12> emptyMap{};
	EXPECT_TRUE(emptyMap.empty());

	bzd::Map<int, int, 12> dataMap{{12, 32}, {1, 2}};
	EXPECT_EQ(dataMap.size(), 2);
}
TEST_CONSTEXPR_END(ContainerMap, Constexpr)
