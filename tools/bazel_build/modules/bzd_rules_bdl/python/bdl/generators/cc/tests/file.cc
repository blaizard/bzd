#include "cc/bzd/test/test.hh"
#include "tools/bdl/generators/cc/tests/manifest.hh"

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
		bzd::test::MyList listA;
		ASSERT_EQ(listA.capacity(), 1U);
		bzd::test::MyListCapacity listB;
		ASSERT_EQ(listB.capacity(), 23U);
	}
}
