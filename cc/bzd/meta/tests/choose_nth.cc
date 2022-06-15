#include "cc/bzd/meta/choose_nth.hh"

#include "cc/bzd/meta/type_list.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/is_same.hh"

TEST(Meta, ChooseNth)
{
	bzd::meta::ChooseNth<0, int, double> a;
	{
		constexpr bool isSame = bzd::typeTraits::isSame<decltype(a), int>;
		EXPECT_TRUE(isSame);
	}

	bzd::meta::ChooseNth<1, int, double> b;
	{
		constexpr bool isSame = bzd::typeTraits::isSame<decltype(b), double>;
		EXPECT_TRUE(isSame);
	}

	using TypeList = bzd::meta::TypeList<int, double>;
	TypeList::ChooseNth<0> c;
	{
		constexpr bool isSame = bzd::typeTraits::isSame<decltype(c), int>;
		EXPECT_TRUE(isSame);
	}
	TypeList::ChooseNth<1> d;
	{
		constexpr bool isSame = bzd::typeTraits::isSame<decltype(d), double>;
		EXPECT_TRUE(isSame);
	}
}
