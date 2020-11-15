#include "bzd/algorithm/fill.h"

#include "bzd/container/array.h"
#include "cc_test/test.h"

TEST(Fill, Base)
{
	bzd::Array<int, 10> array;

	bzd::algorithm::fill(array.begin(), array.end(), 42);

	for (const auto& value : array)
	{
		EXPECT_EQ(value, 42);
	}
}
