#include "cc/bzd/type_traits/decay.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/is_same.hh"

template <typename T, typename U>
struct decayEquiv : bzd::typeTraits::IsSame<bzd::typeTraits::Decay<T>, U>
{
};

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
		const auto ret = decayEquiv<int(int), int (*)(int)>::value;
		EXPECT_TRUE(ret);
	}
}
