#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/iterator.hh"

/*
static_assert(is_iterator_v<std::vector<int>::iterator>);
static_assert(is_iterator_v<std::list<double>::const_iterator>);
static_assert(is_iterator_v<int*>);
static_assert(!is_iterator_v<std::list<double>>);
static_assert(!is_iterator_v<int>);
*/

TEST(TypeTraits, isIterator)
{
	{
		const bool result = bzd::concepts::iterator<int>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::concepts::iterator<char*>;
		EXPECT_TRUE(result);
	}
}
