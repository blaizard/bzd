#include "gtest/gtest.h"

#include "bzd/type_traits/is_constructible.h"

struct Foo
{
    Foo(int) {}
};

TEST(TypeTraits, isConstructible)
{
    {
        auto result = bzd::typeTraits::isConstructible<Foo, int>::value;
        EXPECT_EQ(result, true);
    }
    {
        auto result = bzd::typeTraits::isConstructible<Foo>::value;
        EXPECT_EQ(result, false);
    }
    {
        auto result = bzd::typeTraits::isConstructible<Foo, int, int>::value;
        EXPECT_EQ(result, false);
    }
}
