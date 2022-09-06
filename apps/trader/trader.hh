#pragma once

#include "cc/bzd.hh"
#include "cc/libs/http/http.hh"

namespace Trader {

template <class Network>
bzd::Async<> run(Network& network)
{
	constexpr bzd::StringView hostname{"www.google.com"_sv};

	co_await !bzd::log::info("Connecting to {}"_csv, hostname);

	bzd::http::Client client{network, hostname, 80};
	co_await !client.request("/hello/world"_sv)
		.header(bzd::http::Header{"hello"_sv, "my friend"_sv})
		.header(bzd::http::Header::host("my friend"_sv))
		.send();

	auto stream = co_await !network.connect(hostname, 80);
	co_await !bzd::log::info("Sending GET /"_csv);
	co_await !toStream(stream,
					   "GET / HTTP/1.1\r\n"
					   "Host: {}\r\n"
					   "User-Agent: bzd\r\n"
					   "Accept: */*\r\n\r\n"_csv,
					   hostname);

	bzd::String<1000U> data;
	data.resize(1000U);
	co_await !bzd::log::info("Receiving..."_csv);
	const auto result = co_await !bzd::async::any(stream.read(data.asBytesMutable()), bzd::timeout(1000_ms));

	co_await !bzd::log::info(data.first(result.size()));

	co_return {};
}

} // namespace Trader
