#include "bzd/container/impl/circular_list.h"

#include "cc_test/test.h"

class DummyElement : public bzd::impl::CircularListElement
{
public:
	DummyElement(bzd::SizeType value) : value_{value} {}
	DummyElement(DummyElement&&) = default;

	bzd::SizeType value_;
};

TEST(CircularList, simple)
{
	DummyElement elements[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	bzd::impl::CircularList<DummyElement> list;
	EXPECT_EQ(list.size(), 0);

	{
		const auto result = list.insert(&elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 1);
	}

	{
		const auto& result = list.get();
		EXPECT_TRUE(result);
		EXPECT_EQ((*result).value_, elements[0].value_);
	}

	{
		const auto result = list.remove(&elements[0]);
		EXPECT_TRUE(result);
		EXPECT_EQ(list.size(), 0);
	}
}
