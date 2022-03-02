#include "cc/bzd/type_traits/is_same_template.hh"

#include "cc/bzd/test/test.hh"

template <class T>
struct TestType
{
};

template <class... Args>
struct TestTypeMulti
{
};

template <class V, class E>
struct TestTypeResultLike
{
};

TEST(TypeTraits, isSameTemplate)
{
	{
		const auto a = TestType<int>{};
		const bool result = bzd::typeTraits::isSameTemplate<decltype(a), TestType>;
		EXPECT_TRUE(result);
	}
	{
		const auto a = TestTypeMulti<int, float>{};
		const bool result = bzd::typeTraits::isSameTemplate<decltype(a), TestTypeMulti>;
		EXPECT_TRUE(result);
	}
	{
		const bool result = bzd::typeTraits::isSameTemplate<int, TestType>;
		EXPECT_FALSE(result);
	}
	{
		const auto a = TestTypeMulti<int, float>{};
		const bool result = bzd::typeTraits::isSameTemplate<decltype(a), TestType>;
		EXPECT_FALSE(result);
	}
	{
		const auto a = TestTypeResultLike<int, float>{};
		const bool result = bzd::typeTraits::isSameTemplate<decltype(a), TestTypeResultLike>;
		EXPECT_TRUE(result);
	}
}
