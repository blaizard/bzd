#include "cc/bzd/type_traits/is_reference.hh"

#include "cc/bzd/test/test.hh"

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
