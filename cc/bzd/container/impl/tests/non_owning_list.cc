#include "bzd/container/impl/tests/non_owning_list.h"

#include "cc_test/test.h"

TEST(NonOwningList, simple)
{
	bzd::test::ListElementMultiContainer elements[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	bzd::test::NonOwningList<bzd::test::ListElementMultiContainer> list;
	EXPECT_EQ(list.size(), 0);
	EXPECT_FALSE(list.front());
	EXPECT_FALSE(list.back());

	{
		const auto result = list.insert(elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 1);
	}

	{
		const auto& result = list.front();
		EXPECT_TRUE(result);
		EXPECT_EQ((*result).value_, elements[0].value_);
	}

	{
		const auto result = list.remove(elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 0);
	}
}
