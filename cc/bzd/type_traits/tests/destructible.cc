#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/is_destructible.hh"

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
		auto result = bzd::typeTraits::isDestructible<int>;
		EXPECT_TRUE(result);
	}
	{
		auto result = bzd::typeTraits::isDestructible<A>;
		EXPECT_TRUE(result);
	}
	{
		auto result = bzd::typeTraits::isDestructible<B>;
		EXPECT_FALSE(result);
	}
	{
		auto result = bzd::typeTraits::isDestructible<C>;
		EXPECT_FALSE(result);
	}
}
