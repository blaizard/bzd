#include "bzd/container/any_reference.h"

#include "cc_test/test.h"

TEST(ContainerAnyReference, simple)
{
	int a = 23;
	bzd::AnyReference ref{a};

	{
		auto result = ref.get<int>();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, 23);
		*result = 12;
		EXPECT_EQ(a, 12);
	}
	{
		const auto result = ref.get<float>();
		EXPECT_FALSE(result);
	}
	{
		const auto result = ref.get<const int>();
		EXPECT_FALSE(result);
	}
	{
		const auto result = ref.get<volatile int>();
		EXPECT_FALSE(result);
	}
}

TEST(ContainerAnyReference, constObject)
{
	const int a = 23;
	bzd::AnyReference ref{a};

	{
		auto result = ref.get<const int>();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, 23);
	}
	{
		auto result = ref.get<int>();
		EXPECT_FALSE(result);
	}
}

TEST(ContainerAnyReference, constAny)
{
	int a = 23;
	const bzd::AnyReference ref{a};

	{
		auto result = ref.get<int>();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, 23);
	}
}
