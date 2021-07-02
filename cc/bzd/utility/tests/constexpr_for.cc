#include "cc/bzd/utility/constexpr_for.h"

#include "cc_test/test.h"

TEST(ConstexprFor, Base)
{
	int counter = 0;
	bzd::constexprFor<0, 10, 1>([&counter](auto i) { counter += i.value; });
	EXPECT_EQ(counter, 45);
}

TEST(ConstexprFor, Step)
{
	int counter = 0;
	bzd::constexprFor<0, 10, 2>([&counter](auto i) { counter += i.value; });
	EXPECT_EQ(counter, 20);
}

TEST(ConstexprFor, Decrease)
{
	int counter = 0;
	bzd::constexprFor<9, -1, -1>([&counter](auto i) { counter += i.value; });
	EXPECT_EQ(counter, 45);
}
