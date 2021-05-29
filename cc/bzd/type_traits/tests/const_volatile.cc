#include "cc/bzd/type_traits/is_same.h"
#include "cc/bzd/type_traits/remove_cv.h"
#include "cc_test/test.h"

TEST(TypeTraits, removeCV)
{
	{
		typedef bzd::typeTraits::RemoveCV<const int> type;
		const bool result = bzd::typeTraits::isSame<int, type>;
		EXPECT_TRUE(result);
	}

	{
		typedef bzd::typeTraits::RemoveCV<volatile int> type;
		const bool result = bzd::typeTraits::isSame<int, type>;
		EXPECT_TRUE(result);
	}

	{
		typedef bzd::typeTraits::RemoveCV<const volatile int> type;
		const bool result = bzd::typeTraits::isSame<int, type>;
		EXPECT_TRUE(result);
	}

	{
		typedef bzd::typeTraits::RemoveCV<const volatile int*> type;
		const bool result = bzd::typeTraits::isSame<const volatile int*, type>;
		EXPECT_TRUE(result);
	}

	{
		typedef bzd::typeTraits::RemoveCV<int* const volatile> type;
		const bool result = bzd::typeTraits::isSame<int*, type>;
		EXPECT_TRUE(result);
	}
}
