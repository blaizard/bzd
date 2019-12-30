#include "gtest/gtest.h"
#include "bzd/core/registry.h"

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

// Registry are used to store logger, communication channels
// Data transfer are used by loggers
// static constexpr const registryNbLog
