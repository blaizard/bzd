#include "cc/libs/http/http.hh"

#include "cc/bzd/test/test.hh"
#include "cc/components/generic/network/tcp/client.hh"
#include "tools/bdl/generators/cc/adapter/context.hh"

namespace bzd::http {

template <class Read, class Write>
struct Config
{
	Read read;
	Write write;
};

TEST_ASYNC(Http, Simple)
{
	auto context = bzd::generator::makeContext(
		Config{.read = [](const auto&&) { return "HTTP/1.1 200 OK\r\n"_sv.asBytes(); }, .write = [](const auto) {}});

	bzd::components::generic::network::tcp::Client network{context};
	bzd::http::Client client{network, test.timer(), "", 1234u};

	auto response = co_await !client.get("/").send();
	bzd::Array<char, 100u> data;
	bzd::ignore = co_await !response.read(data.asBytesMutable());

	co_return {};
}

} // namespace bzd::http
