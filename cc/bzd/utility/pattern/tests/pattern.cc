#include "cc/bzd/utility/pattern/pattern.hh"

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/test/test.hh"

class TestSchema
{
public:
	struct Metadata
	{
		bzd::Size index{};
	};

	template <class Adapter>
	static constexpr void parse(Metadata&, bzd::StringView& pattern) noexcept
	{
		pattern.removePrefix(1);
	}

	template <class Adapter, class ValueType>
	static constexpr void check(const Metadata&) noexcept
	{
	}
};

class TestAssert
{
public:
	static void onError(const bzd::StringView&) {}
};

using TestAdapater = bzd::pattern::impl::Adapter<TestAssert, TestSchema>;

TEST(Pattern, ParseStaticString)
{
	{
		bzd::StringView str("Hello");
		const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("Hello {}");
		const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_TRUE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello "_sv);
		EXPECT_EQ(str.front(), '{');
	}

	{
		bzd::StringView str("{}");
		const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_TRUE(result.isMetadata);
		EXPECT_TRUE(result.str.empty());
		EXPECT_EQ(str.front(), '{');
	}

	{
		bzd::StringView str("Hello {{");
		const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello {"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("{{{{");
		const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "{"_sv);
		EXPECT_EQ(str, "{{"_sv);
		const auto result2 = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result2.isMetadata);
		EXPECT_EQ(result2.str, "{"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("}}}}");
		const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "}"_sv);
		EXPECT_EQ(str, "}}"_sv);
		const auto result2 = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result2.isMetadata);
		EXPECT_EQ(result2.str, "}"_sv);
		EXPECT_TRUE(str.empty());
	}
}
/*
TEST(Format_, ParseMetadataIndex)
{
	{
		bzd::StringView str("}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str, 0);
		EXPECT_EQ(metadata.index, 0U);
	}

	{
		bzd::StringView str("}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str, 5);
		EXPECT_EQ(metadata.index, 5U);
	}

	{
		bzd::StringView str(":}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str, 2);
		EXPECT_EQ(metadata.index, 2U);
	}

	{
		bzd::StringView str("10}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str, 2);
		EXPECT_EQ(metadata.index, 10U);
	}

	{
		bzd::StringView str("7:}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str, 8);
		EXPECT_EQ(metadata.index, 7U);
	}
}
*/

TEST(Format, Parser)
{
	{
		[[maybe_unused]] constexpr auto context = bzd::pattern::impl::parse<TestAdapater, decltype("Hello"_csv)>();
	}
	{
		[[maybe_unused]] constexpr auto context = bzd::pattern::impl::parse<TestAdapater, decltype("Hello {}"_csv)>();
	}
	{
		[[maybe_unused]] constexpr auto context = bzd::pattern::impl::parse<TestAdapater, decltype("Hello {} you {} {}{}"_csv)>();
	}
}
