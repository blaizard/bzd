#include "cc/bzd/algorithm/fill.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST_CONSTEXPR_BEGIN(Fill, Constexpr)
{
	bzd::Array<int, 10> array;

	bzd::algorithm::fill(array.begin(), array.end(), 42);

	for (const auto& value : array)
	{
		EXPECT_EQ(value, 42);
	}
}
TEST_CONSTEXPR_END(Fill, Constexpr)
