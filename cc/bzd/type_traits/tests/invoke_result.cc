#include "cc/bzd/type_traits/invoke_result.hh"

#include "cc/bzd/container/function.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc_test/test.hh"

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

void voidFctWithArgs(int) {}

int complexFctWithArgs(double, char*)
{
	return 0;
}

template <class T>
auto temmplateFct(const T& a)
{
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
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(voidFctWithArgs), int>, void>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(complexFctWithArgs), double, char*>, int>;
		EXPECT_TRUE(result);
	}
	{
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(temmplateFct<float>), float>, float>;
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
	{
		bzd::Function<void(int)> callable{[](int) {}};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable), int>, void>;
		EXPECT_TRUE(result);
	}
	{
		bzd::Function<float(char*, double)> callable{[](char*, double) -> float { return 0.f; }};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable), char*, double>, float>;
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
	{
		auto callable = [](int) {};
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable), int>, void>;
		EXPECT_TRUE(result);
	}
	{
		auto callable = [](int&, char*) -> int { return 0; };
		constexpr bool result = bzd::typeTraits::isSame<bzd::typeTraits::InvokeResult<decltype(callable), int&, char*>, int>;
		EXPECT_TRUE(result);
	}
}
