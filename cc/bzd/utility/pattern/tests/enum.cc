#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/to_string.hh"

enum class HelloEnum
{
	zero = 0,
	first = 1,
	third = 3,
	tens = 10
};

TEST(ToString, Enum)
{
	bzd::String<20> str;

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::zero);
		ASSERT_TRUE(result);
		EXPECT_EQ(result.value(), 8U);
		EXPECT_EQ_RANGE(str, "zero (0)"_sv);
	}

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::first);
		ASSERT_TRUE(result);
		EXPECT_EQ(result.value(), 9U);
		EXPECT_EQ_RANGE(str, "first (1)"_sv);
	}

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::third);
		ASSERT_TRUE(result);
		EXPECT_EQ(result.value(), 13U);
		EXPECT_EQ_RANGE(str, "<unknown> (3)"_sv);
	}

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::tens);
		ASSERT_TRUE(result);
		EXPECT_EQ(result.value(), 14U);
		EXPECT_EQ_RANGE(str, "<unknown> (10)"_sv);
	}
}
