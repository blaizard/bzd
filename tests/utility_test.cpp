#include "gtest/gtest.h"
#include "include/utility.h"

template <typename T, typename U>
struct decayEquiv : bzd::typeTraits::isSame<typename bzd::decay<T>::type, U>::type {};

TEST(Utility, decay)
{
	{
		const auto ret1 = decayEquiv<int, int>::value;
    	EXPECT_TRUE(ret1);
		const auto ret2 = decayEquiv<int, double>::value;
    	EXPECT_FALSE(ret2);
	}

	{
		const auto ret = decayEquiv<int&, int>::value;
    	EXPECT_TRUE(ret);
	}

	{
		const auto ret = decayEquiv<int&&, int>::value;
    	EXPECT_TRUE(ret);
	}

	{
		const auto ret = decayEquiv<const int&, int>::value;
    	EXPECT_TRUE(ret);
	}

	{
		const auto ret = decayEquiv<int[2], int*>::value;
    	EXPECT_TRUE(ret);
	}

	{
		const auto ret = decayEquiv<int(int), int(*)(int)>::value;
    	EXPECT_TRUE(ret);
	}
}
