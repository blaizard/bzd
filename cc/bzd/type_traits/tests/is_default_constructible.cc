#include "cc/bzd/type_traits/is_default_constructible.h"

#include "cc_test/test.h"

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
