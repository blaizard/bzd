#include "gtest/gtest.h"

#include "include/type_traits/is_integral.h"

TEST(TypeTraits, isIntegral)
{
	{
		const bool result = bzd::typeTraits::isIntegral<int>::value;
		EXPECT_TRUE(result);
	}
	{
		const bool result = bzd::typeTraits::isIntegral<char*>::value;
		EXPECT_FALSE(result);
	}
}
