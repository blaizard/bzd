#include "bzd/utility/format/format.h"

#include "bzd/container/string_channel.h"
#include "bzd/container/string_view.h"
#include "bzd/container/vector.h"
#include "cc_test/test.h"

#include <stdexcept>

class ThrowAssert
{
public:
	static void onError(const bzd::StringView& message) { throw std::runtime_error(message.data()); }
};

using TestAdapater = bzd::format::impl::Adapter<ThrowAssert>;

TEST(Format_, ParseStaticString)
{
	{
		bzd::StringView str("Hello");
		const auto result = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("Hello {}");
		const auto result = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_TRUE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello "_sv);
		EXPECT_EQ(str.front(), '{');
	}

	{
		bzd::StringView str("{}");
		const auto result = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_TRUE(result.isMetadata);
		EXPECT_TRUE(result.str.empty());
		EXPECT_EQ(str.front(), '{');
	}

	{
		bzd::StringView str("Hello {{");
		const auto result = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "Hello {"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("{{{{");
		const auto result = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "{"_sv);
		EXPECT_EQ(str, "{{"_sv);
		const auto result2 = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result2.isMetadata);
		EXPECT_EQ(result2.str, "{"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("}}}}");
		const auto result = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result.isMetadata);
		EXPECT_EQ(result.str, "}"_sv);
		EXPECT_EQ(str, "}}"_sv);
		const auto result2 = bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_FALSE(result2.isMetadata);
		EXPECT_EQ(result2.str, "}"_sv);
		EXPECT_TRUE(str.empty());
	}

	{
		bzd::StringView str("Hello {");
		EXPECT_ANY_THROW(bzd::format::impl::parseStaticString<TestAdapater>(str));
	}

	{
		bzd::StringView str("} ");
		EXPECT_ANY_THROW(bzd::format::impl::parseStaticString<TestAdapater>(str));
	}
}

TEST(Format_, ParseMetadata)
{
	{
		bzd::StringView str("hello");
		EXPECT_ANY_THROW(bzd::format::impl::parseMetadata<TestAdapater>(str, 0));
	}
}

TEST(Format_, ParseMetadataIndex)
{
	{
		bzd::StringView str("}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 0);
		EXPECT_EQ(metadata.index, 0);
	}

	{
		bzd::StringView str("}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 5);
		EXPECT_EQ(metadata.index, 5);
	}

	{
		bzd::StringView str(":}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 2);
		EXPECT_EQ(metadata.index, 2);
	}

	{
		bzd::StringView str("10}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 2);
		EXPECT_EQ(metadata.index, 10);
	}

	{
		bzd::StringView str("7:}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 8);
		EXPECT_EQ(metadata.index, 7);
	}
}

TEST(Format_, ParseMetadataSign)
{
	{
		bzd::StringView str("}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 0);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::AUTO);
	}

	{
		bzd::StringView str(":-}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 0);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::ONLY_NEGATIVE);
	}

	{
		bzd::StringView str(":+}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 0);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::ALWAYS);
	}
}

TEST(Format_, ParseMetadataPrecision)
{
	{
		bzd::StringView str(":.3f}");
		auto metadata = bzd::format::impl::parseMetadata<TestAdapater>(str, 0);
		EXPECT_TRUE(metadata.isPrecision);
		EXPECT_EQ(metadata.precision, 3);
	}
}

TEST(Format_, StringFormat)
{
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {:d}"), 12);
		EXPECT_STREQ(stream.str().data(), "Hello 12");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {1} {0:d}"), 12, -89);
		EXPECT_STREQ(stream.str().data(), "Hello -89 12");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {:f}"), 12.45);
		EXPECT_STREQ(stream.str().data(), "Hello 12.45");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {:.3f}"), 12.45);
		EXPECT_STREQ(stream.str().data(), "Hello 12.45");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {:%}"), 0.15);
		EXPECT_STREQ(stream.str().data(), "Hello 15.%");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {}"), "World");
		EXPECT_STREQ(stream.str().data(), "Hello World");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("Hello {:.2}"), "World");
		EXPECT_STREQ(stream.str().data(), "Hello Wo");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("This {1} is {0:.1%}"), 0.0349, "milk");
		EXPECT_STREQ(stream.str().data(), "This milk is 3.5%");
	}
	{
		bzd::StringChannel<256> stream;
		bzd::format::toStream(stream, CSTR("{} == {0:#b} == {0:#o} == {0:#x} == {0:#X}"), 42);
		EXPECT_STREQ(stream.str().data(), "42 == 0b101010 == 0o52 == 0x2a == 0x2A");
	}
}
