#include "cc/bzd/container/impl/tests/non_owning_list_for_test.hh"
#include "cc/bzd/test/test.hh"

TEST(NonOwningList, simple)
{
	bzd::test::ListElementMultiContainer elements[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	bzd::test::NonOwningList<bzd::test::ListElementMultiContainer> list;
	EXPECT_EQ(list.size(), 0);
	EXPECT_FALSE(list.front());
	EXPECT_FALSE(list.back());

	{
		const auto result = list.pushFront(elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 1);
	}

	{
		const auto& result = list.front();
		EXPECT_TRUE(result);
		EXPECT_EQ(result->value_, elements[0].value_);
	}

	{
		const auto result = list.pop(elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 0);
		const auto result2 = list.pop(elements[0]);
		EXPECT_FALSE(result2);
		EXPECT_EQ(result2.error(), bzd::ListErrorType::elementAlreadyRemoved);
	}

	{
		const auto result = list.pushFront(elements[2]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 1);

		const auto result2 = elements[2].pop();
		EXPECT_TRUE(result2);
		EXPECT_EQ(list.size(), 0);

		const auto result3 = elements[2].pop();
		EXPECT_FALSE(result3);
		EXPECT_EQ(list.size(), 0);
	}
}
