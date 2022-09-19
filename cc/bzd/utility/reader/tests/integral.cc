#include "cc/bzd/utility/reader/integral.hh"

#include "cc/bzd/test/test.hh"

TEST(FromString, Integer)
{
	{
		bzd::Int32 n;
		const auto result = fromString("312"_sv, n);
		EXPECT_EQ(result, 3u);
		EXPECT_EQ(n, 312);
	}
	{
		bzd::Int32 n;
		const auto result = fromString("-312"_sv, n);
		EXPECT_EQ(result, 4u);
		EXPECT_EQ(n, -312);
	}
	{
		bzd::UInt32 n;
		const auto result = fromString("-312"_sv, n);
		EXPECT_EQ(result, 0u);
	}
	{
		bzd::Int32 n;
		const auto result = fromString("-"_sv, n);
		EXPECT_EQ(result, 0u);
	}
}
