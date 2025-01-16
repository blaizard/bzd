#include "cc/libs/http/http.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/ranges/views/drop_last.hh"
#include "cc/bzd/utility/ranges/views_async/join.hh"
#include "cc/components/generic/network/tcp/client.hh"
#include "cc/components/generic/network/tcp/tests/mock.hh"
#include "cc/libs/http/tests/data/request_http1.1_chunked.hh"
#include "cc/libs/http/tests/data/request_http1.1_chunked_expected.hh"

namespace bzd::http {

TEST_ASYNC(Http, Simple)
{
	auto network = bzd::components::generic::network::tcp::MockClientWithString("HTTP/1.1 200 OK\r\n\r\n"_sv.asBytes());
	bzd::http::Client client{network, test.timer(), "", 1234u};

	[[maybe_unused]] auto response = co_await !client.get("/").send();

	// Consume the output, should be empty.
	bzd::Array<char, 100u> data;
	EXPECT_EQ_RANGE(response.reader(data.asBytesMutable()) | bzd::ranges::join(), ""_sv.asBytes());

	co_return {};
}

TEST_ASYNC(Http, Chunked, (bzd::Array<char, 1000u>, bzd::Array<char, 10u>, bzd::Array<char, 1u>))
{
	auto network = bzd::components::generic::network::tcp::MockClientWithString(bzd::StringView{request_http1_1_chunked}.asBytes());
	bzd::http::Client client{network, test.timer(), "", 1234u};

	auto response = co_await !client.get("/").send();

	TestType data;
	auto values = response.reader(data.asBytesMutable()) | bzd::ranges::join();
	EXPECT_EQ_RANGE(values, bzd::StringView{request_http1_1_chunked_expected}.asBytes());
	co_return {};
}

} // namespace bzd::http
