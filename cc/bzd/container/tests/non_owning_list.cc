#include "cc/bzd/container/non_owning_list.hh"

#include "cc/bzd/test/test.hh"

class ElementType : public bzd::NonOwningListElement
{
public:
	ElementType(const bzd::Int32 value) : value_{value} {}
	ElementType() = default;

	bzd::Int32 value_{0};
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
	list.clear();

	EXPECT_TRUE(list.pushFront(elements[0]));
	EXPECT_TRUE(list.pushFront(elements[1]));
	EXPECT_TRUE(list.pushFront(elements[2]));
	EXPECT_TRUE(list.pushFront(elements[3]));
	EXPECT_EQ(list.size(), 4U);
	list.clear();
	EXPECT_TRUE(list.empty());
}

TEST(NonOwningList, Iterator)
{
	ElementType elements[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	NonOwningListForTest list;

	EXPECT_EQ(list.begin(), list.end());

	EXPECT_TRUE(list.pushFront(elements[0]));
	EXPECT_TRUE(list.pushFront(elements[1]));
	EXPECT_TRUE(list.pushFront(elements[2]));
	EXPECT_TRUE(list.pushFront(elements[3]));
	EXPECT_EQ(list.size(), 4U);

	auto it = list.begin();
	EXPECT_EQ(it->value_, 3);
	++it;
	EXPECT_EQ(it->value_, 2);
	++it;
	EXPECT_EQ(it->value_, 1);
	++it;
	EXPECT_EQ(it->value_, 0);
	++it;
	EXPECT_EQ(it, list.end());

	bzd::UInt32 counter{0};
	for ([[maybe_unused]] auto& item : list)
	{
		++counter;
	}
	EXPECT_EQ(counter, 4U);
}

TEST(NonOwningList, ConstIterator)
{
	ElementType elements[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	NonOwningListForTest list;
	const NonOwningListForTest& clist{list};

	EXPECT_EQ(clist.begin(), clist.end());

	EXPECT_TRUE(list.pushFront(elements[0]));
	EXPECT_TRUE(list.pushFront(elements[1]));
	EXPECT_TRUE(list.pushFront(elements[2]));
	EXPECT_TRUE(list.pushFront(elements[3]));
	EXPECT_EQ(list.size(), 4U);

	auto it = clist.begin();
	EXPECT_EQ(it->value_, 3);
	++it;
	EXPECT_EQ(it->value_, 2);
	++it;
	EXPECT_EQ(it->value_, 1);
	++it;
	EXPECT_EQ(it->value_, 0);
	++it;
	EXPECT_EQ(it, clist.end());

	bzd::UInt32 counter{0};
	for ([[maybe_unused]] const auto& item : clist)
	{
		++counter;
	}
	EXPECT_EQ(counter, 4U);
}
