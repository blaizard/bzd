#include "cc/bzd/type_traits/is_class.hh"

#include "cc/bzd/test/test.hh"

struct A
{
};

class B
{
};

enum class C
{
};

TEST(TypeTraits, isClass)
{
	{
		const bool result = bzd::typeTraits::isClass<int>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::typeTraits::isClass<A>;
		EXPECT_TRUE(result);
	}
	{
		const bool result = bzd::typeTraits::isClass<B>;
		EXPECT_TRUE(result);
	}
	{
		const bool result = bzd::typeTraits::isClass<C>;
		EXPECT_FALSE(result);
	}
}
