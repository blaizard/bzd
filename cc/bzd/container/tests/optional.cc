#include "bzd/container/optional.h"

#include "bzd/container/result.h"
#include "cc_test/test.h"

TEST(ContainerOptional, simpleData)
{
	bzd::Optional<int> v(42);

	EXPECT_TRUE(v);
	EXPECT_EQ(*v, 42);
	EXPECT_EQ(v.valueOr(10), 42);

	*v = 13;
	EXPECT_EQ(*v, 13);

	v.emplace(45);
	EXPECT_TRUE(v);
	EXPECT_EQ(*v, 45);

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
	EXPECT_EQ(*v, 42);
	a = -85;
	EXPECT_EQ(*v, -85);

	int b = 12;
	v.emplace(b);
	EXPECT_TRUE(v);
	EXPECT_EQ(*v, 12);
	b = -25;
	EXPECT_EQ(*v, -25);
}

TEST(ContainerOptional, pointer)
{
	int a = 42;
	bzd::Optional<int*> v(&a);

	EXPECT_TRUE(v);
	EXPECT_EQ(*(*v), 42);
	a = -85;
	EXPECT_EQ(*(*v), -85);
}

TEST(ContainerOptional, result)
{
	{
		bzd::Optional<bzd::Result<int, bool>> v({45});

		EXPECT_TRUE(v);
		EXPECT_EQ(*v.valueOr({12}), 45);
	}

	{
		bzd::Optional<bzd::Result<int, bool>> v{};

		EXPECT_FALSE(v);
		EXPECT_EQ(*v.valueOr({12}), 12);

		v.emplace(43);
		EXPECT_TRUE(v);
		EXPECT_TRUE(*v);
		EXPECT_EQ(*v.valueOr({12}), 43);

		v.emplace(bzd::makeError(false));
		EXPECT_TRUE(v);
		EXPECT_FALSE(*v);
		EXPECT_FALSE((*v).error());
	}
}
