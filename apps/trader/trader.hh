#pragma once

#include "cc/bzd.hh"
#include "cc/libs/http/http.hh"

namespace trader {

template <class Network>
bzd::Async<> run(Network& network)
{
	constexpr bzd::StringView hostname{"www.google.com"_sv};

	co_await !bzd::log::info("Connecting to {}"_csv, hostname);

	bzd::http::Client client{network, hostname, 80};
	auto response = co_await !client.get("/"_sv).header("Host", hostname).header("User-Agent", "bzd").header("Accept", "*/*").send();

	co_await !bzd::log::info("Receiving..."_csv);

	bzd::Array<bzd::Byte, 1000u> data;
	const auto result = co_await !bzd::async::any(response.read(data.asBytesMutable()), bzd::platform::steadyClock().timeout(1000_ms));
	co_await !bzd::log::info(bzd::StringView{reinterpret_cast<const char*>(result.data()), result.size()});

	co_return {};
}

} // namespace trader
