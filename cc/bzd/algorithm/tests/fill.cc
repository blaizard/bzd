#include "cc/bzd/algorithm/fill.hh"

#include "cc/bzd/container/array.hh"
#include "cc_test/test.hh"

TEST(Fill, Base)
{
	bzd::Array<int, 10> array;

	bzd::algorithm::fill(array.begin(), array.end(), 42);

	for (const auto& value : array)
	{
		EXPECT_EQ(value, 42);
	}
}
