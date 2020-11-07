#include "bzd/type_traits/invoke_result.h"

#include "bzd/container/function.h"
#include "bzd/type_traits/is_same.h"
#include "cc_test/test.h"

int intFct()
{
	return 0;
}

void voidFct() {}

void* pointerFct()
{
	return nullptr;
}

const void* constPointerFct()
{
	return nullptr;
}

int& referenceFct()
{
	static int a = 0;
	return a;
}

const int& constReferenceFct()
{
	static const int a = 0;
	return a;
}

TEST(InvokeResult, rawFunctions)
{
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(intFct)>, int>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(voidFct)>, void>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(pointerFct)>, void*>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(constPointerFct)>, const void*>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(referenceFct)>, int&>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(constReferenceFct)>, const int&>;
		EXPECT_TRUE(result);
	}
}

TEST(InvokeResult, functions)
{
	{
		bzd::Function<void(void)> callable{[]() {}};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, void>;
		EXPECT_TRUE(result);
	}
	{
		bzd::Function<int(void)> callable{[]() -> int { return 0; }};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, int>;
		EXPECT_TRUE(result);
	}
	{
		bzd::Function<void*(void)> callable{[]() -> void* { return nullptr; }};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, void*>;
		EXPECT_TRUE(result);
	}
	{
		bzd::Function<const void*(void)> callable{[]() -> const void* { return nullptr; }};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, const void*>;
		EXPECT_TRUE(result);
	}
	{
		bzd::Function<int&(void)> callable{[]() -> int& {
			static int a = 0;
			return a;
		}};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, int&>;
		EXPECT_TRUE(result);
	}
	{
		bzd::Function<const int&(void)> callable{[]() -> const int& {
			static int a = 0;
			return a;
		}};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, const int&>;
		EXPECT_TRUE(result);
	}
}

TEST(InvokeResult, lambda)
{
	{
		auto callable = []() {};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, void>;
		EXPECT_TRUE(result);
	}
	{
		auto callable = []() -> int { return 0; };
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, int>;
		EXPECT_TRUE(result);
	}
	{
		auto callable = []() -> void* { return nullptr; };
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, void*>;
		EXPECT_TRUE(result);
	}
	{
		auto callable = []() -> const void* { return nullptr; };
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, const void*>;
		EXPECT_TRUE(result);
	}
	{
		auto callable = []() -> int& {
			static int a = 0;
			return a;
		};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, int&>;
		EXPECT_TRUE(result);
	}
	{
		auto callable = []() -> const int& {
			static int a = 0;
			return a;
		};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable)>, const int&>;
		EXPECT_TRUE(result);
	}
}
