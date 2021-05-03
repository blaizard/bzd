#include "bzd/container/optional.h"

#include "bzd/container/result.h"
#include "cc_test/test.h"

TEST(ContainerOptional, simpleData)
{
	bzd::Optional<int> v(42);

	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 42);
	EXPECT_EQ(v.valueOr(10), 42);

	v.valueMutable() = 13;
	EXPECT_EQ(v.value(), 13);

	v.emplace(45);
	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 45);

	v.reset();
	EXPECT_FALSE(v);
}

TEST(ContainerOptional, simpleNoData)
{
	bzd::Optional<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOr(10), 10);
}

TEST(ContainerOptional, constexprType)
{
	constexpr bzd::Optional<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOr(10), 10);

	constexpr bzd::Optional<int> u{23};

	EXPECT_TRUE(u);
	EXPECT_EQ(u.valueOr(10), 23);
}

TEST(ContainerOptional, complexData)
{
	struct Value
	{
		int a;
	};
	bzd::Optional<Value> v{45};

	EXPECT_TRUE(v);
	EXPECT_EQ(v.valueOr({12}).a, 45);
	EXPECT_EQ(v->a, 45);

	v.emplace(Value{23});
	EXPECT_TRUE(v);
	EXPECT_EQ(v->a, 23);

	v.reset();
	EXPECT_FALSE(v);
}

TEST(ContainerOptional, reference)
{
	int a = 42;
	bzd::Optional<int&> v(a);

	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 42);
	a = -85;
	EXPECT_EQ(v.value(), -85);

	int b = 12;
	v.emplace(b);
	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 12);
	b = -25;
	EXPECT_EQ(v.value(), -25);
}

TEST(ContainerOptional, pointer)
{
	int a = 42;
	bzd::Optional<int*> v(&a);

	EXPECT_TRUE(v);
	EXPECT_EQ(*(v.value()), 42);
	a = -85;
	EXPECT_EQ(*(v.value()), -85);
}

TEST(ContainerOptional, result)
{
	{
		bzd::Optional<bzd::Result<int, bool>> v({45});

		EXPECT_TRUE(v);
		EXPECT_EQ(v.valueOr({12}).value(), 45);
	}

	{
		bzd::Optional<bzd::Result<int, bool>> v{};

		EXPECT_FALSE(v);
		EXPECT_EQ(v.valueOr({12}).value(), 12);

		v.emplace(43);
		EXPECT_TRUE(v);
		EXPECT_TRUE(v.value());
		EXPECT_EQ(v.valueOr({12}).value(), 43);

		v.emplace(bzd::makeError(false));
		EXPECT_TRUE(v);
		EXPECT_FALSE(v.value());
		EXPECT_FALSE((v.value()).error());
	}
}
