#include "gtest/gtest.h"
#include "bzd/container/map.h"

TEST(ContainerMap, single)
{
	bzd::Map<int, int, 12> map;

	map.insert(12, 5);
}
