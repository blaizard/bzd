#include "cc/libs/http/http.hh"

#include "cc/bdl/generator/impl/adapter/context.hh"
#include "cc/bzd/test/test.hh"
#include "cc/components/generic/network/tcp/client.hh"
#include "cc/libs/http/tests/data/request_http1.1_chunked.hh"

namespace bzd::http {

template <class Read, class Write>
struct Config
{
	Read read;
	Write write;
};

template <class BytesSpan>
class MockRead
{
public:
	explicit constexpr MockRead(const BytesSpan data) noexcept : data_{data} {}

	bzd::Span<const bzd::Byte> operator()(const auto&& input) const noexcept
	{
		const auto maximumSize = input.data.size();
		const auto outputSize = bzd::min(maximumSize, data_.size() - index_);
		const auto output = data_.subSpan(index_, outputSize);
		index_ += output.size();
		return output;
	}

private:
	const BytesSpan data_;
	mutable bzd::Size index_{0};
};

TEST_ASYNC(Http, Simple)
{
	auto context =
		bzd::generator::makeContext(Config{.read = MockRead{"HTTP/1.1 200 OK\r\n\r\n"_sv.asBytes()}, .write = [](const auto) {}});

	bzd::components::generic::network::tcp::Client network{context};
	bzd::http::Client client{network, test.timer(), "", 1234u};

	auto response = co_await !client.get("/").send();
	bzd::Array<char, 100u> data;
	bzd::ignore = co_await !response.read(data.asBytesMutable());

	co_return {};
}

TEST_ASYNC(Http, Chunked)
{
	auto context = bzd::generator::makeContext(
		Config{.read = MockRead{bzd::StringView{request_http1_1_chunked}.asBytes()}, .write = [](const auto) {}});

	bzd::components::generic::network::tcp::Client network{context};
	bzd::http::Client client{network, test.timer(), "", 1234u};

	auto response = co_await !client.get("/").send();
	bzd::Array<char, 100u> data;
	bzd::ignore = co_await !response.read(data.asBytesMutable());

	co_return {};
}

} // namespace bzd::http
