#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/panic.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/pattern.hh"

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

static bool failed{false};

class SetBoolean
{
public:
	static void onError(const bzd::StringView&) noexcept { failed = true; }
};

using TestAdapaterSetBoolean = bzd::pattern::impl::Adapter<TestSchema, SetBoolean>;

TEST(Pattern, MalformedPattern)
{
	{
		bzd::StringView str("Hello {");
		bzd::pattern::impl::parseStaticString<TestAdapaterSetBoolean>(str);
		EXPECT_TRUE(failed);
		failed = false;
	}

	{
		bzd::StringView str("} ");
		bzd::pattern::impl::parseStaticString<TestAdapaterSetBoolean>(str);
		EXPECT_TRUE(failed);
		failed = false;
	}
}
