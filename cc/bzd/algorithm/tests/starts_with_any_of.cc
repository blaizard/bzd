#include "cc/bzd/algorithm/starts_with_any_of.hh"
#include "cc/bzd/test/test.hh"

TEST(EqualAnyOf, Base)
{
	const auto map = {"abc"_sv, "acd"_sv, "bbddee"_sv, "bde"_sv, "gshj"_sv};
	{
		const auto it = bzd::algorithm::startsWithAnyOf("gshj"_sv, map);
		EXPECT_NE(it, map.end());
		EXPECT_STREQ(it->data(), "gshj");
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("abc"_sv, map);
		EXPECT_NE(it, map.end());
		EXPECT_STREQ(it->data(), "abc");
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("acd"_sv, map);
		EXPECT_NE(it, map.end());
		EXPECT_STREQ(it->data(), "acd");
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("bbddee"_sv, map);
		EXPECT_NE(it, map.end());
		EXPECT_STREQ(it->data(), "bbddee");
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("bde"_sv, map);
		EXPECT_NE(it, map.end());
		EXPECT_STREQ(it->data(), "bde");
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("gshj"_sv, map);
		EXPECT_NE(it, map.end());
		EXPECT_STREQ(it->data(), "gshj");
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("bbdded"_sv, map);
		EXPECT_EQ(it, map.end());
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("ab"_sv, map);
		EXPECT_EQ(it, map.end());
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf(""_sv, map);
		EXPECT_EQ(it, map.end());
	}
	{
		const auto it = bzd::algorithm::startsWithAnyOf("helloIamaverylongstring"_sv, map);
		EXPECT_EQ(it, map.end());
	}
}
