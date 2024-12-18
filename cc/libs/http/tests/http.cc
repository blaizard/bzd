#include "cc/libs/http/http.hh"

#include "cc/bzd/test/test.hh"
#include "cc/components/generic/network/tcp/client.hh"
#include "cc/components/generic/network/tcp/tests/mock.hh"
#include "cc/libs/http/tests/data/request_http1.1_chunked.hh"

namespace bzd::http {

TEST_ASYNC(Http, Simple)
{
	auto network = bzd::components::generic::network::tcp::MockClientWithString("HTTP/1.1 200 OK\r\n\r\n"_sv.asBytes());
	bzd::http::Client client{network, test.timer(), "", 1234u};

	auto response = co_await !client.get("/").send();
	bzd::Array<char, 100u> data;
	bzd::ignore = co_await !response.read(data.asBytesMutable());

	co_return {};
}

TEST_ASYNC(Http, Chunked)
{
	auto network = bzd::components::generic::network::tcp::MockClientWithString(bzd::StringView{request_http1_1_chunked}.asBytes());
	bzd::http::Client client{network, test.timer(), "", 1234u};

	auto response = co_await !client.get("/").send();
	bzd::Array<char, 100u> data;
	bzd::ignore = co_await !response.read(data.asBytesMutable());

	co_return {};
}

} // namespace bzd::http
