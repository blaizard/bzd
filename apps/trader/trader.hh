#pragma once

#include "cc/bzd.hh"

namespace Trader {

template <class Network>
bzd::Async<> run(Network& network)
{
	const bzd::StringView hostname{"www.google.com"_sv};

	co_await !bzd::log::info("Connecting to {}"_csv, hostname);
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
	// Timeout is not supported yet for functions that use bzd:::async::suspend
	// This is because the asynt goes out of the scope of its executor.
	// const auto result = co_await !bzd::async::any(stream.read(data.asBytesMutable()), bzd::timeout(500_ms));
	const auto result = co_await !stream.read(data.asBytesMutable());

	co_await !bzd::log::info(data.first(result.size()));

	co_return {};
}

} // namespace Trader
