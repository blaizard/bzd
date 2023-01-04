#include "example/terminal/reader.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/components/generic/stream/stub/stub.hh"

TEST_ASYNC(Reader, readUntil)
{
	struct Config {};
	bzd::platform::generic::stream::Stub out{Config{}};
	bzd::test::IStream<16> mock{};
	bzd::Reader<16> reader{mock, out};

	mock << "Hello:you";
	const auto result = co_await reader.readUntil(bzd::Byte{':'});
	EXPECT_TRUE(result);
	EXPECT_EQ_VALUES(result.value(), "Hello:"_sv.asBytes());

	co_return {};
}
