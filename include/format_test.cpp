#include "gtest/gtest.h"
#include "include/format.h"
#include "include/container/string_stream.h"
#include "include/container/string_view.h"
#include "include/container/vector.h"

class ContextTest
{
public:
	void addSubstring(const bzd::StringView& str) { substrings_.pushBack(str); }
	void onError(const bzd::StringView& message) const { throw std::runtime_error(message.data()); }

	bzd::Vector<bzd::String<100>, 10> substrings_;
};
using Context = bzd::impl::format::Context<ContextTest>;

TEST(Format_, ParseStaticString)
{
	{
		Context ctx;
		bzd::StringView str("Hello");
		bzd::impl::format::parseStaticString(ctx, str);
		EXPECT_EQ(ctx.substrings_.size(), 1);
		EXPECT_STREQ(ctx.substrings_[0].data(), "Hello");
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("Hello {}");
		bzd::impl::format::parseStaticString(ctx, str);
		EXPECT_EQ(ctx.substrings_.size(), 1);
		EXPECT_STREQ(ctx.substrings_[0].data(), "Hello ");
		EXPECT_EQ(str.front(), '{');
	}

	{
		Context ctx;
		bzd::StringView str("{}");
		bzd::impl::format::parseStaticString(ctx, str);
		EXPECT_EQ(ctx.substrings_.size(), 0);
		EXPECT_EQ(str.front(), '{');
	}

	{
		Context ctx;
		bzd::StringView str("Hello {{");
		bzd::impl::format::parseStaticString(ctx, str);
		EXPECT_EQ(ctx.substrings_.size(), 1);
		EXPECT_STREQ(ctx.substrings_[0].data(), "Hello {");
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("{{{{");
		bzd::impl::format::parseStaticString(ctx, str);
		EXPECT_EQ(ctx.substrings_.size(), 2);
		EXPECT_STREQ(ctx.substrings_[0].data(), "{");
		EXPECT_STREQ(ctx.substrings_[1].data(), "{");
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("}}}}");
		bzd::impl::format::parseStaticString(ctx, str);
		EXPECT_EQ(ctx.substrings_.size(), 2);
		EXPECT_STREQ(ctx.substrings_[0].data(), "}");
		EXPECT_STREQ(ctx.substrings_[1].data(), "}");
		EXPECT_TRUE(str.empty());
	}

	{
		Context ctx;
		bzd::StringView str("Hello {");
		EXPECT_ANY_THROW(bzd::impl::format::parseStaticString(ctx, str));
	}

	{
		Context ctx;
		bzd::StringView str("} ");
		EXPECT_ANY_THROW(bzd::impl::format::parseStaticString(ctx, str));
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
		EXPECT_EQ(metadata.sign, bzd::impl::format::Metadata::Sign::AUTO);
	}

	{
		Context ctx;
		bzd::StringView str(":-}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_EQ(metadata.sign, bzd::impl::format::Metadata::Sign::ONLY_NEGATIVE);
	}

	{
		Context ctx;
		bzd::StringView str(":+}");
		auto metadata = parseMetadata(ctx, str, 0);
		EXPECT_EQ(metadata.sign, bzd::impl::format::Metadata::Sign::ALWAYS);
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
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {:d}"), 12);
		EXPECT_STREQ(stream.str().data(), "Hello 12");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {1} {0:d}"), 12, -89);
		EXPECT_STREQ(stream.str().data(), "Hello -89 12");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {:f}"), 12.45);
		EXPECT_STREQ(stream.str().data(), "Hello 12.45");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {:.3f}"), 12.45);
		EXPECT_STREQ(stream.str().data(), "Hello 12.45");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {:%}"), 0.15);
		EXPECT_STREQ(stream.str().data(), "Hello 15.%");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {}"), "World");
		EXPECT_STREQ(stream.str().data(), "Hello World");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {:.2}"), "World");
		EXPECT_STREQ(stream.str().data(), "Hello Wo");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("This {1} is {0:.1%}"), 0.0349, "milk");
		EXPECT_STREQ(stream.str().data(), "This milk is 3.5%");
	}
	{
		bzd::StringStream<256> stream;
		bzd::format(stream, CONSTEXPR_STRING_VIEW("{} == {0:#b} == {0:#o} == {0:#x} == {0:#X}"), 42);
		EXPECT_STREQ(stream.str().data(), "42 == 0b101010 == 0o52 == 0x2a == 0x2A");
	}
}
