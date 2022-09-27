#include "cc/bzd/utility/pattern/reader/from_string.hh"

#include "cc/bzd/core/panic.hh"
#include "cc/bzd/test/test.hh"

class TestAssert : public bzd::reader::impl::Schema
{
public:
	static void onError(const bzd::StringView& str)
	{
		::std::cout << "Error at: " << str.data() << ::std::endl;
		bzd::Panic::trigger();
	}
};

using TestAdapater = bzd::pattern::impl::Adapter<TestAssert>;

TEST(FromString, NoArguments)
{
	const bzd::StringView str{"Hello World"};
	const auto result = fromString(str, "Hello"_csv);
	EXPECT_FALSE(result);
}

TEST(FromString, SimpleArguments)
{
	// const bzd::StringView str{"Number 12"};

	// bzd::StringView str{"{}"};

	bzd::pattern::impl::Parser<TestAdapater> parser{"Hello {}"};
	for (auto result : parser)
	{
		(void)result;
	}

	// const auto result = bzd::pattern::impl::parseStaticString<TestAdapater>(str);
	/*
		bzd::UInt32 a;
		const auto result = fromString(str, "{}"_csv, a);*/
	// EXPECT_TRUE(result.isMetadata);
}
