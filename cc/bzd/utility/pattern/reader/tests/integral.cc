#include "cc/bzd/utility/pattern/reader/integral.hh"

#include "cc/bzd/test/test.hh"

TEST(IntegralFromString, Integer)
{
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("312"_sv, n);
		EXPECT_TRUE(result);
		EXPECT_EQ(n, 312);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("-312"_sv, n);
		EXPECT_TRUE(result);
		EXPECT_EQ(n, -312);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("-312yes"_sv, n);
		EXPECT_TRUE(result);
		EXPECT_EQ(n, -312);
		EXPECT_EQ(result.value(), 4u);
	}
	{
		bzd::UInt32 n;
		const auto result = bzd::fromString("-312"_sv, n);
		EXPECT_FALSE(result);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("-"_sv, n);
		EXPECT_FALSE(result);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString(""_sv, n);
		EXPECT_FALSE(result);
	}
}
