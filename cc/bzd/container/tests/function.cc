#include "cc/bzd/container/function.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/is_same.hh"

TEST(ContainerFunction, nonCapturingLambda)
{
	{
		static int i = 0;
		bzd::Function<void(void)> callable{[]() { i = 12; }};
		callable();
		EXPECT_EQ(i, 12);
	}
	{
		bzd::Function<int(void)> callable{[]() -> int { return 14; }};
		auto result = callable();
		EXPECT_EQ(result, 14);
		const bool isSameType = bzd::typeTraits::isSame<decltype(result), int>;
		EXPECT_TRUE(isSameType);
	}
	{
		bzd::Function<int(int)> callable{[](int a) -> int { return a; }};
		auto result = callable(78);
		EXPECT_EQ(result, 78);
		const bool isSameType = bzd::typeTraits::isSame<decltype(result), int>;
		EXPECT_TRUE(isSameType);
	}
	{
		bzd::Function<double(double, int)> callable{[](double a, int b) -> int { return a + b; }};
		auto result = callable(11, 12);
		EXPECT_EQ(result, 23);
		const bool isSameType = bzd::typeTraits::isSame<decltype(result), double>;
		EXPECT_TRUE(isSameType);
	}
}

TEST(ContainerFunction, capturingLambda)
{
	{
		int j = 0;
		bzd::Function<void(void)> callable{[&j]() { j = -5; }};
		callable();
		EXPECT_EQ(j, -5);
	}
	{
		int j = 0;
		bzd::Function<void(int)> callable{[&j](int a) { j = a; }};
		callable(42);
		EXPECT_EQ(j, 42);
	}
}

TEST(ContainerFunction, tags)
{
	int j = 0;
	bzd::Function<void(void)> callable1{[&j]() { j = -1; }};
	bzd::Function<void(void)> callable2{[&j]() { j = -5; }};
	callable1();
	EXPECT_EQ(j, -1);
	callable2();
	EXPECT_EQ(j, -5);

	// copy
	callable2 = callable1;
	callable2();
	EXPECT_EQ(j, -1);
	callable1();
	EXPECT_EQ(j, -1);
}

static int rawFunctionTestVariable_ = 0;
void rawFunction()
{
	rawFunctionTestVariable_ = 58;
}

TEST(ContainerFunction, rawFunction)
{
	bzd::Function<void(void)> callable{rawFunction};
	callable();
	EXPECT_EQ(rawFunctionTestVariable_, 58);
}
