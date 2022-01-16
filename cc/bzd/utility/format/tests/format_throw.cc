#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/format/format.hh"
#include "cc/bzd/utility/format/stream.hh"

#include <stdexcept>

class ThrowAssert
{
public:
	static void onError(const bzd::StringView& message) { throw std::runtime_error(message.data()); }
};

using TestAdapater = bzd::format::impl::Adapter<ThrowAssert>;

TEST(Format_, ParseStaticStringThrow)
{
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
