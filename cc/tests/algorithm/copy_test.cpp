#include "bzd/algorithm/copy.h"
#include "bzd/container/array.h"
#include "cc_test/test.h"

TEST(Copy, Base)
{
	bzd::Array<int, 10> arraySrc{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	bzd::Array<int, 10> arrayDst;

	bzd::algorithm::copy(arraySrc.begin(), arraySrc.end(), arrayDst.begin());

	int exepected = 0;
	for (const auto& value : arrayDst)
	{
		EXPECT_EQ(value, exepected);
		++exepected;
	}
}
