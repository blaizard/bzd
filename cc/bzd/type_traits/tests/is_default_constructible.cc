#include "cc/bzd/type_traits/is_default_constructible.hh"

#include "cc/bzd/test/test.hh"

struct Foo
{
	Foo(int) {}
};

struct FooDefault
{
	FooDefault() {}
};

TEST(TypeTraits, isDefaultConstructible)
{
	{
		auto result = bzd::typeTraits::isDefaultConstructible<Foo>;
		EXPECT_FALSE(result);
	}
	{
		auto result = bzd::typeTraits::isDefaultConstructible<FooDefault>;
		EXPECT_TRUE(result);
	}
}
