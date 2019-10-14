#include "gtest/gtest.h"
#include "include/format_.h"
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

TEST(Format_, StringFormat)
{
	bzd::StringStream<256> stream;
	bzd::format(stream, CONSTEXPR_STRING_VIEW("Hello {:i}"), 12);

	//EXPECT_STREQ(stream.str().data(), "Hello 12");
}
