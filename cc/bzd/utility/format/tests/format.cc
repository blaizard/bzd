#include "cc/bzd/utility/format/format.hh"

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/format/stream.hh"

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

template <bzd::SizeType N, class... Args>
void expectStringStreamFormat(const char* expected, Args&&... args)
{
	bzd::String<N> str;
	toString(str, bzd::forward<Args>(args)...);
	EXPECT_STREQ(str.data(), expected);

	bzd::StringStream<N> stream;
	toStream(stream, bzd::forward<Args>(args)...).sync();
	EXPECT_STREQ(stream.str().data(), expected);
}

TEST(Format_, StreamStringFormat)
{
	expectStringStreamFormat<256>("Hello 12", CSTR("Hello {:d}"), 12);
	expectStringStreamFormat<256>("Hello -89 12", CSTR("Hello {1} {0:d}"), 12, -89);
	expectStringStreamFormat<256>("Hello 12.45", CSTR("Hello {:f}"), 12.45);
	expectStringStreamFormat<256>("Hello 12.5", CSTR("Hello {:.1f}"), 12.45);
	expectStringStreamFormat<256>("Hello 15.%", CSTR("Hello {:%}"), 0.15);
	expectStringStreamFormat<256>("Hello World", CSTR("Hello {}"), "World");
	expectStringStreamFormat<256>("Hello Wo", CSTR("Hello {:.2}"), "World");
	expectStringStreamFormat<256>("This milk is 3.5%", CSTR("This {1} is {0:.1%}"), 0.0349, "milk");
	expectStringStreamFormat<256>("42 == 0b101010 == 0o52 == 0x2a == 0x2A", CSTR("{} == {0:#b} == {0:#o} == {0:#x} == {0:#X}"), 42);
	expectStringStreamFormat<256>("Pointer 0x1234", CSTR("Pointer {}"), reinterpret_cast<const void*>(0x1234));
}
