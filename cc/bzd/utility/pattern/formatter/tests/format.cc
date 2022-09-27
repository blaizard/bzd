#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/panic.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/pattern/formatter/to_stream.hh"
#include "cc/bzd/utility/pattern/formatter/to_string.hh"

class TestAssert : public bzd::format::impl::SchemaFormat
{
public:
	static void onError(const bzd::StringView&) { bzd::Panic::trigger(); }
};

using TestAdapater = bzd::pattern::impl::Adapter<TestAssert>;

TEST(Format_, ParseMetadataSign)
{
	{
		bzd::StringView str("}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::AUTO);
	}

	{
		bzd::StringView str("-}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::ONLY_NEGATIVE);
	}

	{
		bzd::StringView str("+}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str);
		EXPECT_EQ(metadata.sign, bzd::format::impl::Metadata::Sign::ALWAYS);
	}
}

TEST(Format_, ParseMetadataPrecision)
{
	{
		bzd::StringView str(".3f}");
		TestAdapater::Metadata metadata{};
		TestAdapater::template parse<TestAdapater>(metadata, str);
		EXPECT_TRUE(metadata.isPrecision);
		EXPECT_EQ(metadata.precision, 3U);
	}
}

template <bzd::Size N, class... Args>
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
	expectStringStreamFormat<256>("Hello 12", "Hello {:d}"_csv, 12);
	expectStringStreamFormat<256>("Hello -89 12", "Hello {1} {0:d}"_csv, 12, -89);
	expectStringStreamFormat<256>("Hello 12.45", "Hello {:f}"_csv, 12.45);
	expectStringStreamFormat<256>("Hello 12.5", "Hello {:.1f}"_csv, 12.45);
	expectStringStreamFormat<256>("Hello 15.%", "Hello {:%}"_csv, 0.15);
	expectStringStreamFormat<256>("Hello World", "Hello {}"_csv, "World");
	expectStringStreamFormat<256>("Hello Wo", "Hello {:.2}"_csv, "World");
	expectStringStreamFormat<256>("This milk is 3.5%", "This {1} is {0:.1%}"_csv, 0.0349, "milk");
	expectStringStreamFormat<256>("42 == 0b101010 == 0o52 == 0x2a == 0x2A", "{} == {0:#b} == {0:#o} == {0:#x} == {0:#X}"_csv, 42);
	expectStringStreamFormat<256>("Pointer 0x1234", "Pointer {}"_csv, reinterpret_cast<const void*>(0x1234));
}

TEST_CONSTEXPR_BEGIN(Format_, Constexpr)
{
	{
		bzd::String<128> str;
		toString(str, "Hello World"_csv);
		EXPECT_STREQ(str.data(), "Hello World");
	}

	{
		bzd::String<128> str;
		toString(str, "Hello {}"_csv, "World");
		EXPECT_STREQ(str.data(), "Hello World");
	}

	{
		bzd::String<128> str;
		toString(str, "Hello {}"_csv, 12);
		EXPECT_STREQ(str.data(), "Hello 12");
	}
}
TEST_CONSTEXPR_END(Format_, Constexpr)
