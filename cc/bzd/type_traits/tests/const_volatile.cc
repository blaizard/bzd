#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cv.hh"

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
