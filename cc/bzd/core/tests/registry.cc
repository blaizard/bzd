#include "cc/bzd/core/registry.hh"

#include "cc_test/test.hh"

TEST(Registry, Base)
{
	bzd::Registry<int>::Declare<3> reg_;
	bzd::Registry<int>::Register<> hello("hello", 12);
	bzd::Registry<int>::Register<> yes("yes", -1);
	bzd::Registry<int>::Register<> me("me", 986);

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

	bzd::Registry<Foo>::Declare<1> reg_;
	bzd::Registry<Foo>::Register<Bar> hello("hello", 12);

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
		int print() override { return 42; }
	};

	bzd::Registry<PureVirtualBase>::Declare<1> reg_;
	bzd::Registry<PureVirtualBase>::Register<PureVirtualSpecialization> hello("hello");

	EXPECT_EQ(bzd::Registry<PureVirtualBase>::get("hello").print(), 42);
}

TEST(Registry, Overflow)
{
	bzd::Registry<float>::Declare<1> reg_;
	bzd::Registry<float>::Register<> m0("0", 12.0f);
	EXPECT_ANY_THROW(bzd::Registry<float>::Register<> m1("1", -1.0f));
}

TEST(Registry, NotDeclared)
{
	EXPECT_ANY_THROW(bzd::Registry<double>::Register<> m1("1", -1.8));
}

TEST(Registry, GetOrCreate)
{
	bzd::Registry<long>::Declare<1> reg_;
	bzd::Registry<long>::Register<> m0("0", 12);
	EXPECT_EQ(bzd::Registry<long>::getOrCreate("hello"), 0);
}
