#include "bzd/type_traits/is_reference.h"

#include "cc_test/test.h"

TEST(TypeTraits, isReference)
{
	{
		auto result = bzd::typeTraits::isReference<int>;
		EXPECT_EQ(result, false);
	}
	{
		auto result = bzd::typeTraits::isReference<int&>;
		EXPECT_EQ(result, true);
	}
	{
		auto result = bzd::typeTraits::isReference<int&&>;
		EXPECT_EQ(result, true);
	}
}
