#include "cc/bzd/utility/pattern/reader/integral.hh"

#include "cc/bzd/test/test.hh"

TEST(Integral, Integer)
{
	{
		bzd::Int32 n;
		const auto result = fromString("312"_sv, n);
		EXPECT_TRUE(result);
		EXPECT_EQ(n, 312);
	}
	{
		bzd::Int32 n;
		const auto result = fromString("-312"_sv, n);
		EXPECT_TRUE(result);
		EXPECT_EQ(n, -312);
	}
	{
		bzd::Int32 n;
		const auto result = fromString("-312yes"_sv, n);
		EXPECT_TRUE(result);
		EXPECT_EQ(n, -312);
		EXPECT_EQ(*(result.value()), 'y');
	}
	{
		bzd::UInt32 n;
		const auto result = fromString("-312"_sv, n);
		EXPECT_FALSE(result);
	}
	{
		bzd::Int32 n;
		const auto result = fromString("-"_sv, n);
		EXPECT_FALSE(result);
	}
	{
		bzd::Int32 n;
		const auto result = fromString(""_sv, n);
		EXPECT_FALSE(result);
	}
}
