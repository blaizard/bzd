#pragma once

#include "cc/bzd.hh"

namespace bzd {

template <class Network>
class Http
{
public:
	constexpr Http(Network& network, const StringView hostname, const UInt32 port) noexcept :
		network_{network}, hostname_{hostname}, port_{port}
	{
	}

	Async<> get(const StringView path) noexcept
	{
		stream_ = co_await !network_.connect(hostname_, port_);
		co_await !toStream(stream_,
						   "GET / HTTP/1.1\r\n"
						   "Host: {}\r\n"
						   "User-Agent: bzd\r\n"
						   "Accept: */*\r\n\r\n"_csv,
						   hostname_);
	}

private:
	Network& network_;
	Optional<typename Network::Stream> stream_;
	StringView hostname_;
	UInt32 port_;
};

} // namespace bzd
