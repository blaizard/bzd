#include "bzd/type_traits/is_constructible.h"

#include "cc_test/test.h"

struct Foo
{
	Foo(int) {}
};

TEST(TypeTraits, isConstructible)
{
	{
		auto result = bzd::typeTraits::isConstructible<Foo, int>;
		EXPECT_EQ(result, true);
	}
	{
		auto result = bzd::typeTraits::isConstructible<Foo>;
		EXPECT_EQ(result, false);
	}
	{
		auto result = bzd::typeTraits::isConstructible<Foo, int, int>;
		EXPECT_EQ(result, false);
	}
}
