#include "cc/bzd/container/threadsafe/tests/non_owning_forward_list_for_test.hh"
#include "cc/bzd/test/test.hh"

TEST(NonOwningForwardList, simple)
{
	bzd::test::ListElementMultiContainer elements[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	bzd::test::NonOwningForwardList<bzd::test::ListElementMultiContainer> list;
	EXPECT_EQ(list.size(), 0U);
	EXPECT_FALSE(list.front());
	EXPECT_FALSE(list.back());

	{
		const auto result = list.pushFront(elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 1U);
	}

	{
		const auto& result = list.front();
		EXPECT_TRUE(result);
		EXPECT_EQ(result->value_, elements[0].value_);
	}

	{
		const auto result = list.pop(elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 0U);
		const auto result2 = list.pop(elements[0]);
		EXPECT_FALSE(result2);
		EXPECT_EQ(result2.error(), bzd::threadsafe::NonOwningForwardListErrorType::elementAlreadyRemoved);
	}

	{
		const auto result = list.pushFront(elements[2]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 1U);

		// Iterating through a list with 1 element.
		for (const auto& elt : list)
		{
			EXPECT_EQ(elt.value_, 3U);
		}

		const auto result2 = elements[2].pop();
		EXPECT_TRUE(result2);
		EXPECT_EQ(list.size(), 0U);

		const auto result3 = elements[2].pop();
		EXPECT_FALSE(result3);
		EXPECT_EQ(list.size(), 0U);

		// Iterating through a list with no elements.
		for ([[maybe_unused]] const auto& elt : list)
		{
			EXPECT_FALSE(true);
		}
	}

	{
		for (int i = 9; i >= 0; --i)
		{
			const auto result = list.pushFront(elements[i]);
			EXPECT_TRUE(result);
		}
		EXPECT_EQ(list.size(), 10U);

		bzd::SizeType expected{1};
		for (const auto& elt : list)
		{
			EXPECT_EQ(elt.value_, expected);
			++expected;
		}

		expected = 1;
		for (const auto& elt : static_cast<const decltype(list)&>(list))
		{
			EXPECT_EQ(elt.value_, expected);
			++expected;
		}
	}
}
