#include "cc/bzd/utility/iterators/advance.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Advance, Base)
{
	bzd::Array<int, 3> hello{bzd::inPlace, 3, 1, 4};

	auto it = hello.begin();
	bzd::advance(it, 2);
	EXPECT_EQ(*it, 4);

	bzd::advance(it, hello.end());
	EXPECT_EQ(it, hello.end());

	bzd::advance(it, -3);
	EXPECT_EQ(*it, 3);

	const auto result1 = bzd::advance(it, 2, hello.end());
	EXPECT_EQ(result1, 0);
	EXPECT_EQ(*it, 4);

	const auto result2 = bzd::advance(it, 4, hello.end());
	EXPECT_EQ(result2, 3);
	EXPECT_EQ(it, hello.end());
}
