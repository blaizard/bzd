#include "cc/bzd/utility/memory/non_null_ptr.hh"

#include "cc/bzd/test/test.hh"

TEST(NonNullPtr, Simple)
{
	int a = 2;
	bzd::NonNullPtr ptr{&a};

	EXPECT_EQ(*ptr, 2);
}
