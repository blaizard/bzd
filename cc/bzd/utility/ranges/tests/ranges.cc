#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/range.hh"
#include "cc/bzd/type_traits/range.hh"

TEST(Ranges, Concepts)
{
	{
		const auto is = bzd::concepts::range<const char[12]>;
		EXPECT_TRUE(is);
	}
	{
		const auto is = bzd::concepts::range<const char*>;
		EXPECT_FALSE(is);
	}
}
