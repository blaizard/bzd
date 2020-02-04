#include "bzd/type_traits/is_integral.h"
#include "cc_test/test.h"

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
