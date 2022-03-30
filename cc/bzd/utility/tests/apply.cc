#include "cc/bzd/utility/apply.hh"

#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/test/test.hh"

TEST(Apply, Base)
{
	bzd::Tuple<int, int, int> tuple{bzd::inPlace, 1, 2, 3};
	const int result = bzd::apply([](auto... elements) -> int { return (elements + ...); }, tuple);
	EXPECT_EQ(result, 6);
}
