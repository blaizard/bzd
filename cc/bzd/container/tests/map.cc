#include "cc/bzd/container/map.hh"

#include "cc_test/test.hh"

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

TEST(ContainerMap, Constructor)
{
	bzd::Map<int, int, 12> emptyMap{};
	EXPECT_TRUE(emptyMap.empty());

	struct Templ
	{
		int x;
		int y;
	};

	[[maybe_unused]] constexpr bzd::Array<Templ, 2> map{{12, 23}, {12, 23}};

	static constexpr bzd::Map<int, int, 12> dataMap{{12, 32}, {1, 2}};
	EXPECT_EQ(dataMap.size(), 2);
}
