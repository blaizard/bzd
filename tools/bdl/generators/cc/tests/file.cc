#include "cc_test/test.h"
#include "tools/bdl/generators/cc/tests/manifest.h"

TEST(File, Compile)
{
	{
		bzd::test::Error error{bzd::test::Error::B};
		ASSERT_NE(error, bzd::test::Error::A);
		ASSERT_EQ(error, bzd::test::Error::B);
	}

	{
		bzd::test::MyStruct s{2, 2.4f};
		ASSERT_EQ(s.a, 2);
		ASSERT_TRUE(s.b);
		ASSERT_NEAR(s.b.value(), 2.4f, 0.01);
	}

	{
		bzd::test::MyInterfaceA::NestedA nestedA{21};
		ASSERT_EQ(nestedA.a, 21);
	}

	{
		bzd::test::MyInterfaceB::NestedB nestedB{3};
		ASSERT_EQ(nestedB.b, 3);
		bzd::test::MyInterfaceB::NestedA nestedAWithinB{6};
		ASSERT_EQ(nestedAWithinB.a, 6);
	}

	{
		bzd::test::MyList listA;
		ASSERT_EQ(listA.capacity(), 1);
		bzd::test::MyListCapacity listB;
		ASSERT_EQ(listB.capacity(), 23);
	}
}
