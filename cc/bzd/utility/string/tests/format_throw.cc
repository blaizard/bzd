#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/string/formatter/to_stream.hh"
#include "cc/bzd/utility/string/formatter/to_string.hh"

static bool failed{false};

class ThrowAssert : public bzd::format::impl::SchemaFormat
{
public:
	static void onError(const bzd::StringView&) noexcept { failed = true; }
};

using TestAdapater = bzd::format::impl::Adapter<ThrowAssert>;

TEST(Format_, ParseStaticStringThrow)
{
	{
		bzd::StringView str("Hello {");
		bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_TRUE(failed);
		failed = false;
	}

	{
		bzd::StringView str("} ");
		bzd::format::impl::parseStaticString<TestAdapater>(str);
		EXPECT_TRUE(failed);
		failed = false;
	}
}

TEST(Format_, ParseMetadata)
{
	{
		bzd::StringView str("hello");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str);
		EXPECT_TRUE(failed);
		failed = false;
	}
}
