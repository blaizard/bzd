#include "cc/bzd/container/range/views/views.hh"

#include "cc/bzd/algorithm/equal.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/range.hh"

TEST(Views, Drop)
{
	bzd::test::Range<bzd::typeTraits::ForwardTag, int, 5> range{0, 1, 2, 3, 4};
	bzd::range::Drop view{range, 2};

	bzd::test::Range<bzd::typeTraits::ForwardTag, int, 3> expected{2, 3, 4};
	const auto isEqual = bzd::algorithm::equal(view, expected);
	EXPECT_TRUE(isEqual);
}
