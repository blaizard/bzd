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
}

TEST(ContainerOptional, simpleNoData)
{
	bzd::Optional<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOr(10), 10);
}

TEST(ContainerOptional, complexData)
{
	struct Value
	{
		int a;
	};
	bzd::Optional<Value> v{Value{45}};

	EXPECT_TRUE(v);
	EXPECT_EQ(v.valueOr({12}).a, 45);
	EXPECT_EQ(v->a, 45);
}

TEST(ContainerOptional, reference)
{
	int a = 42;
	bzd::Optional<int&> v(a);

	EXPECT_TRUE(v);
	EXPECT_EQ(*v, 42);
	a = -85;
	EXPECT_EQ(*v, -85);
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
	}
}
