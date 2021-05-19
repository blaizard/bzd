#include "bzd/utility/format/format.h"

#include "bzd/container/string_channel.h"
#include "bzd/container/string_view.h"
#include "bzd/container/vector.h"
#include "cc_test/test.h"

#include <stdexcept>

class ContextTest
{
public:
	void processSubstring(const bzd::StringView& str) { substrings_.emplaceBack(str); }
	void onError(const bzd::StringView& message) const { throw std::runtime_error(message.data()); }

	bzd::Vector<bzd::String<100>, 10> substrings_;
};
using Context = bzd::format::impl::Context<ContextTest>;

TEST(Format_, ParseStaticString)
{
	{
		Context ctx;
		bzd::StringView str("Hello");
		const auto result = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("Hello {}");
		const auto result = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_TRUE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello "_sv);
		EXPECT_EQ(str.front(), '{');
	}

	{
		Context ctx;
		bzd::StringView str("{}");
		const auto result = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_TRUE(result.isMetadata);
		EXPECT_TRUE(result.str.empty());
		EXPECT_EQ(str.front(), '{');
	}

	{
		Context ctx;
		bzd::StringView str("Hello {{");
		const auto result = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello {"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("{{{{");
		const auto result = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "{"_sv);
		EXPECT_EQ(str, "{{"_sv);
		const auto result2 = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_FALSE(result2.isMetadata);
		EXPECT_EQ(result2.str, "{"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("}}}}");
		const auto result = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "}"_sv);
		EXPECT_EQ(str, "}}"_sv);
		const auto result2 = bzd::format::impl::parseStaticString(ctx, str);
		EXPECT_FALSE(result2.isMetadata);
		EXPECT_EQ(result2.str, "}"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("Hello {");
		EXPECT_ANY_THROW(bzd::format::impl::parseStaticString(ctx, str));
	}

	{
		Context ctx;
		bzd::StringView str("} ");
		EXPECT_ANY_THROW(bzd::format::impl::parseStaticString(ctx, str));
	}
}

TEST(Format_, ParseMetadata)
{
	{
		Context ctx;
		bzd::StringView str("hello");
		EXPECT_ANY_THROW(parseMetadata(ctx, str, 0));
	}
}

TEST(Format_, ParseMetadataIndex)
{
	{
		Context ctx;
		bzd::StringView str("}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_EQ(metadata.index, 0);
	}

	{
		Context ctx;
		bzd::StringView str("}");
		auto metadata = parseMetadata(ctx, str, 5);
		EXPECT_EQ(metadata.index, 5);
	}

	{
		Context ctx;
		bzd::StringView str(":}");
		auto metadata = parseMetadata(ctx, str, 2);
		EXPECT_EQ(metadata.index, 2);
	}

	{
		Context ctx;
		bzd::StringView str("10}");
		auto metadata = parseMetadata(ctx, str, 2);
		EXPECT_EQ(metadata.index, 10);
	}

	{
		Context ctx;
		bzd::StringView str("7:}");
		auto metadata = parseMetadata(ctx, str, 8);
		EXPECT_EQ(metadata.index, 7);
	}
}

TEST(Format_, ParseMetadataSign)
{
	{
		Context ctx;
		bzd::StringView str("}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::AUTO);
	}

	{
		Context ctx;
		bzd::StringView str(":-}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::ONLY_NEGATIVE);
	}

	{
		Context ctx;
		bzd::StringView str(":+}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::ALWAYS);
	}
}

TEST(Format_, ParseMetadataPrecision)
{
	{
		Context ctx;
		bzd::StringView str(":.3f}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_TRUE(metadata.isPrecision);
		EXPECT_EQ(metadata.precision, 3);
	}
}

TEST(Format_, StringFormat)
{
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {:d}"_sv, 12);
		EXPECT_STREQ(stream.str().data(), "Hello 12");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {1} {0:d}"_sv, 12, -89);
		EXPECT_STREQ(stream.str().data(), "Hello -89 12");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {:f}"_sv, 12.45);
		EXPECT_STREQ(stream.str().data(), "Hello 12.45");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {:.3f}"_sv, 12.45);
		EXPECT_STREQ(stream.str().data(), "Hello 12.45");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {:%}"_sv, 0.15);
		EXPECT_STREQ(stream.str().data(), "Hello 15.%");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {}"_sv, "World");
		EXPECT_STREQ(stream.str().data(), "Hello World");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "Hello {:.2}"_sv, "World");
		EXPECT_STREQ(stream.str().data(), "Hello Wo");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "This {1} is {0:.1%}"_sv, 0.0349, "milk");
		EXPECT_STREQ(stream.str().data(), "This milk is 3.5%");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toString(stream, "{} == {0:#b} == {0:#o} == {0:#x} == {0:#X}"_sv, 42);
		EXPECT_STREQ(stream.str().data(), "42 == 0b101010 == 0o52 == 0x2a == 0x2A");
	}
}
