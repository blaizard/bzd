#include "cc/bzd/container/any_reference.hh"

#include "cc_test/test.hh"

TEST(ContainerAnyReference, simple)
{
	int a = 23;
	bzd::AnyReference ref{a};

	{
		auto result = ref.get<int>();
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 23);
		result.valueMutable() = 12;
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
		EXPECT_EQ(result.value(), 23);
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
		EXPECT_EQ(result.value(), 23);
	}
	{
		auto result = ref.get<const int>();
		EXPECT_FALSE(result);
	}
}

TEST(ContainerAnyReference, cast)
{
	int a = 23;
	bzd::AnyReference ref{a};
	auto& aRef = ref.cast<int>();
	EXPECT_EQ(aRef, 23);
}

TEST(ContainerAnyReference, constCast)
{
	int a = 23;
	const bzd::AnyReference ref{a};
	auto& aRef = ref.cast<int>();
	EXPECT_EQ(aRef, 23);
}

TEST(ContainerAnyReference, interference)
{
	int a = 23;
	int b = 12;
	const bzd::AnyReference refA{a};
	const bzd::AnyReference refB{b};
	int u = refA.cast<int>() + refB.cast<int>();
	EXPECT_EQ(u, 35);
}