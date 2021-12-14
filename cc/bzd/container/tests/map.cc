#include "cc/bzd/container/map.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerMap, single)
{
	bzd::Map<int, int, 12> map;

	map.insert(12, 5);

	{
		const auto it = map.find(42);
		EXPECT_EQ(it, map.end());
	}
	{
		const auto it = map.find(12);
		EXPECT_NE(it, map.end());
		EXPECT_EQ(it->second, 5);
		EXPECT_EQ(map[12], 5);
	}
}

TEST(ContainerMap, find)
{
	bzd::Map<int, int, 12> map{
		{0, 0},
		{1, 0},
		{3, 0},
		{4, 0},
		{5, 0},
		{10, 0},
	};

	{
		const auto it = map.find(-1);
		EXPECT_EQ(it, map.end());
	}
	{
		const auto it = map.find(0);
		EXPECT_NE(it, map.end());
		EXPECT_EQ(it->first, 0);
	}
	{
		const auto it = map.find(1);
		EXPECT_NE(it, map.end());
		EXPECT_EQ(it->first, 1);
	}
	{
		const auto it = map.find(2);
		EXPECT_EQ(it, map.end());
	}
	{
		const auto it = map.find(3);
		EXPECT_NE(it, map.end());
		EXPECT_EQ(it->first, 3);
	}
	{
		const auto it = map.find(10);
		EXPECT_NE(it, map.end());
		EXPECT_EQ(it->first, 10);
	}
	{
		const auto it = map.find(11);
		EXPECT_EQ(it, map.end());
	}
}

TEST(ContainerMap, lessCompare)
{
	struct KeyTest
	{
		bool operator<(const KeyTest& other) const noexcept
		{
			return a < other.a;
		}
		int a;
	};
	bzd::Map<KeyTest, int, 4> map{
		{KeyTest{0}, 0},
		{KeyTest{1}, 0},
	};
	{
		const auto it = map.find(KeyTest{42});
		EXPECT_EQ(it, map.end());
	}
	map.insert(KeyTest{42}, 4);
	{
		const auto it = map.find(KeyTest{42});
		EXPECT_NE(it, map.end());
		EXPECT_EQ(it->first.a, 42);
		EXPECT_EQ(it->second, 4);
	}
}

TEST(ContainerMap, sorted)
{
	bzd::Map<int, int, 10> map{{9, 5}};

	map.insert(12, 12);
	map.insert(1, 3);
	map.insert(0, 2);
	map.insert(12, 6);
	map.insert(13, 7);
	map.insert(-3, 1);
	map.insert(8, 4);
	map.insert(80, 8);
	map.insert(233, 9);
	map.insert(-7, 0);

	int previousKey = -100;
	int expectedValue = 0;
	for (const auto it : map)
	{
		EXPECT_GT(it.first, previousKey);
		EXPECT_EQ(it.second, expectedValue);
		previousKey = it.first;
		++expectedValue;
	}
}

TEST_CONSTEXPR_BEGIN(ContainerMap, Constexpr)
{
	bzd::Map<int, int, 12> emptyMap{};
	EXPECT_TRUE(emptyMap.empty());

	bzd::Map<int, int, 12> dataMap{{12, 32}, {1, 2}, {4, 8}, {-31, 4}, {122, 0}};
	EXPECT_EQ(dataMap.size(), 5);

	EXPECT_EQ(dataMap[4], 8);
}
TEST_CONSTEXPR_END(ContainerMap, Constexpr)
