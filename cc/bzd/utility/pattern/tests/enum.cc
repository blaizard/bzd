#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/to_string.hh"

enum class HelloEnum
{
	first = 1,
	third = 3,
	tens = 10
};

TEST(ToString, Enum)
{
	bzd::String<10> str;

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::first);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1U);
		EXPECT_EQ_RANGE(str, "1"_sv);
	}

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::third);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 1U);
		EXPECT_EQ_RANGE(str, "3"_sv);
	}

	{
		const auto result = bzd::toString(str.assigner(), HelloEnum::tens);
		EXPECT_TRUE(result);
		EXPECT_EQ(result.value(), 2U);
		EXPECT_EQ_RANGE(str, "10"_sv);
	}
}
