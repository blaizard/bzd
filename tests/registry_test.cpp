#include "bzd/core/registry.h"

#include "gtest/gtest.h"

TEST(Registry, Base)
{
	bzd::declare::Registry<int, 3> reg_;
	bzd::Registry<int> hello("hello", 12);
	bzd::Registry<int> yes("yes", -1);
	bzd::Registry<int> me("me", 986);

	EXPECT_EQ(bzd::Registry<int>::get("hello"), 12);
	EXPECT_EQ(bzd::Registry<int>::get("me"), 986);
	EXPECT_EQ(bzd::Registry<int>::get("yes"), -1);
}

TEST(Registry, Inheritance)
{
	struct Foo
	{
		int a;
	};

	struct Bar : Foo
	{
		Bar(int a_) : Foo{a_} {}
	};

	bzd::declare::Registry<Foo, 1> reg_;
	bzd::Registry<Foo, Bar> hello("hello", 12);

	EXPECT_EQ(bzd::Registry<Foo>::get("hello").a, 12);
}

TEST(Registry, PureVirtual)
{
	struct PureVirtualBase
	{
		virtual int print() = 0;
	};

	struct PureVirtualSpecialization : PureVirtualBase
	{
		int print() override { return 42; };
	};

	bzd::declare::Registry<PureVirtualBase, 1> reg_;
	bzd::Registry<PureVirtualBase, PureVirtualSpecialization> hello("hello");

	EXPECT_EQ(bzd::Registry<PureVirtualBase>::get("hello").print(), 12);
}

TEST(Registry, Overflow)
{
	bzd::declare::Registry<float, 1> reg_;
	bzd::Registry<float> m0("0", 12);
	EXPECT_ANY_THROW(bzd::Registry<float> m1("1", -1));
}

TEST(Registry, NotDeclared)
{
	EXPECT_ANY_THROW(bzd::Registry<double> m1("1", -1));
}
