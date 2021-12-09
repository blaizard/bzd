#include "cc/bzd/type_traits/is_iterator.hh"

#include "cc/bzd/test/test.hh"

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
		const bool result = bzd::typeTraits::isIterator<int>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::typeTraits::isIterator<char*>;
		EXPECT_TRUE(result);
	}
}
