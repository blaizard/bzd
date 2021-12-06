#include "cc/bzd/type_traits/is_constructible.hh"

#include "cc/bzd/test/test.hh"

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
