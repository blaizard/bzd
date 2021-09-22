#include "cc/bzd/type_traits/is_base_of.hh"

#include "cc_test/test.hh"

struct A
{
};

class B : public A
{
};

TEST(TypeTraits, isBaseOf)
{
	{
		const bool result = bzd::typeTraits::isBaseOf<A, B>;
		EXPECT_TRUE(result);
	}
	{
		const bool result = bzd::typeTraits::isBaseOf<B, A>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::typeTraits::isBaseOf<A, int>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::typeTraits::isBaseOf<double, int>;
		EXPECT_FALSE(result);
	}
}
