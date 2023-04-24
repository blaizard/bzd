#pragma once

#include "cc/bzd.hh"
#include "cc/libs/http/http.hh"

namespace trader {

template <class Context>
class Trader
{
public:
	explicit constexpr Trader(Context& context) noexcept : context_{context}, logger_{context_.config.out} {}

	bzd::Async<> run()
	{
		constexpr bzd::StringView hostname{"www.google.com"_sv};

		co_await !logger_.info("Connecting to {}"_csv, hostname);

		bzd::http::Client client{context_.config.client, hostname, 80};
		auto response = co_await !client.get("/"_sv).header("Host", hostname).header("User-Agent", "bzd").header("Accept", "*/*").send();

		co_await !logger_.info("Receiving..."_csv);

		bzd::Array<bzd::Byte, 1000u> data;
		const auto result = co_await !bzd::async::any(response.read(data.asBytesMutable()), context_.config.steadyClock.timeout(1000_ms));
		co_await !logger_.info(bzd::StringView{reinterpret_cast<const char*>(result.data()), result.size()});

		co_return {};
	}

private:
	Context& context_;
	bzd::Logger logger_;
};

} // namespace trader
