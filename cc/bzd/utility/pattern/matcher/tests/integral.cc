#include "cc/bzd/utility/pattern/matcher/integral.hh"

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

TEST(IntegralFromString, Hexadecimal)
{
	using Metadata = typename bzd::Matcher<bzd::Int32>::Metadata;

	{
		bzd::Int32 n;
		const auto result = bzd::fromString("312"_sv, n, Metadata{Metadata::Format::hexadecimalLower});
		EXPECT_TRUE(result);
		EXPECT_EQ(n, 786);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("3a2"_sv, n, Metadata{Metadata::Format::hexadecimalLower});
		EXPECT_TRUE(result);
		EXPECT_EQ(n, 930);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("3F2"_sv, n, Metadata{Metadata::Format::hexadecimalUpper});
		EXPECT_TRUE(result);
		EXPECT_EQ(n, 1010);
	}
	{
		bzd::Int32 n;
		const auto result = bzd::fromString("f2"_sv, n, Metadata{Metadata::Format::hexadecimalUpper});
		EXPECT_FALSE(result);
	}
}
