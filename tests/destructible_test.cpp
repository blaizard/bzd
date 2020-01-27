#include "bzd/type_traits/is_destructible.h"

#include "cc_test/test.h"

TEST(TypeTraits, isDestructible)
{
	struct A
	{
	};
	struct B
	{
		~B() = delete;
	};
	struct C : B
	{
	};

	{
		auto result = bzd::typeTraits::isDestructible<int>::value;
		EXPECT_TRUE(result);
	}
	{
		auto result = bzd::typeTraits::isDestructible<A>::value;
		EXPECT_TRUE(result);
	}
	{
		auto result = bzd::typeTraits::isDestructible<B>::value;
		EXPECT_FALSE(result);
	}
	{
		auto result = bzd::typeTraits::isDestructible<C>::value;
		EXPECT_FALSE(result);
	}
}
