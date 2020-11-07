#include "bzd/type_traits/is_enum.h"

#include "cc_test/test.h"

TEST(TypeTraits, isEnum)
{
	{
		const bool result = bzd::typeTraits::isEnum<int>;
		EXPECT_FALSE(result);
	}
	{
		struct Trivial
		{
			int val;
		};
		const bool result = bzd::typeTraits::isEnum<Trivial>;
		EXPECT_FALSE(result);
	}
	{
		union Ints {
			int in;
			long lo;
		};
		const bool result = bzd::typeTraits::isEnum<Ints>;
		EXPECT_FALSE(result);
	}
	{
		enum MyEnum
		{
			HELLO,
			WORLD
		};
		const bool result = bzd::typeTraits::isEnum<MyEnum>;
		EXPECT_TRUE(result);
	}
}
