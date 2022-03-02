#include "cc/bzd/container/non_owning_list.hh"

#include "cc/bzd/test/test.hh"

class ElementType : public bzd::NonOwningListElement
{
public:
	bzd::Int32Type value_{0};
};

using NonOwningListForTest = bzd::NonOwningList<ElementType>;

TEST(NonOwningList, Simple)
{
	ElementType elements[10];
	NonOwningListForTest list;

	EXPECT_EQ(list.size(), 0U);
	{
		const auto result = list.pushFront(elements[0]);
		EXPECT_TRUE(result);
	}
	EXPECT_EQ(list.size(), 1U);
	{
		const auto result = list.pushBack(elements[1]);
		EXPECT_TRUE(result);
	}
	EXPECT_EQ(list.size(), 2U);
	{
		const auto result = list.pushBack(elements[1]);
		EXPECT_FALSE(result);
	}
	EXPECT_EQ(list.size(), 2U);
	{
		const auto result = list.erase(elements[1]);
		EXPECT_TRUE(result);
	}
	EXPECT_EQ(list.size(), 1U);
	{
		const auto result = list.erase(elements[1]);
		EXPECT_FALSE(result);
	}
	EXPECT_EQ(list.size(), 1U);
	{
		const auto result = list.popBack();
		EXPECT_TRUE(result);
	}
	EXPECT_TRUE(list.empty());
	{
		const auto result = list.popFront();
		EXPECT_FALSE(result);
	}
	EXPECT_TRUE(list.empty());
}
