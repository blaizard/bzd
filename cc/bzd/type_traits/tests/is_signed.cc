#include "cc/bzd/type_traits/is_signed.h"

#include "cc_test/test.h"

TEST(TypeTraits, isSigned)
{
	{
		const bool result = bzd::typeTraits::isSigned<int>;
		EXPECT_TRUE(result);
	}
	{
		const bool result = bzd::typeTraits::isSigned<unsigned int>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::typeTraits::isSigned<bool>;
		EXPECT_FALSE(result);
	}
	{
		const bool result = bzd::typeTraits::isSigned<char[12]>;
		EXPECT_FALSE(result);
	}
}
