#include "example/terminal/reader.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/istream.hh"
#include "cc/components/generic/stream/stub/stub.hh"

TEST_ASYNC(Reader, readUntil)
{
	bzd::platform::generic::stream::Stub out{};
	bzd::test::IStream<16> mock{};
	bzd::Reader<16> reader{mock, out};

	mock << "Hello:you";
	const auto result = co_await reader.readUntil(bzd::ByteType{':'});
	EXPECT_TRUE(result);
	// EXPECT_EQ(result.value(), "Hello"_sv.asBytes());
}
