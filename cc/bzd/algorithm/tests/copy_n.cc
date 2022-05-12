#include "cc/bzd/algorithm/copy_n.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST_CONSTEXPR_BEGIN(Copy, Constexpr)
{
	bzd::Array<int, 10> arraySrc{bzd::inPlace, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	bzd::Array<int, 5> arrayDst;

	bzd::algorithm::copyN(arraySrc.begin(), 5, arrayDst.begin());

	int expected = 0;
	for (const auto& value : arrayDst)
	{
		EXPECT_EQ(value, expected);
		++expected;
	}
}
TEST_CONSTEXPR_END(Copy, Constexpr)
