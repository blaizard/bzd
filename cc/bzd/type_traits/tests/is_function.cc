#include "cc/bzd/type_traits/is_function.hh"

#include "cc/bzd/test/test.hh"

int f();

struct A
{
	int fun() const&;
};

template <typename>
struct PM_traits
{
};

template <class T, class U>
struct PM_traits<U T::*>
{
	using member_type = U;
};

TEST(TypeTraits, isFunction)
{
	{
		const auto isFunction = bzd::typeTraits::isFunction<A>;
		EXPECT_FALSE(isFunction);
	}

	{
		const auto isFunction = bzd::typeTraits::isFunction<int(int)>;
		EXPECT_TRUE(isFunction);
	}

	{
		const auto isFunction = bzd::typeTraits::isFunction<decltype(f)>;
		EXPECT_TRUE(isFunction);
	}

	{
		const auto isFunction = bzd::typeTraits::isFunction<int>;
		EXPECT_FALSE(isFunction);
	}

	{
		using T = PM_traits<decltype(&A::fun)>::member_type; // T is int() const&
		const auto isFunction = bzd::typeTraits::isFunction<T>;
		EXPECT_TRUE(isFunction);
	}
}
