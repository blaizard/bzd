#pragma once

#include "cc/bzd.hh"

namespace bzd {

template <Size n = 1>
class Request
{
public:
	constexpr Request<n + 1> header() && { return {}; }

	Async<> send() { co_return {}; }

private:
	const Map<StringView, StringView, n> headers_;
};

// Support for HTTP/1.0 & HTTP/1.1
template <class Network>
class Http
{
public:
	constexpr Http(Network& network, const StringView hostname, const UInt32 port) noexcept :
		network_{network}, hostname_{hostname}, port_{port}
	{
	}

	constexpr Request<> request() { return {}; }

	Async<> get(const StringView path, const interface::Map<StringView, StringView>) noexcept
	{
		stream_ = co_await !network_.connect(hostname_, port_);
		co_await !toStream(stream_.valueMutable(),
						   "GET {} HTTP/1.1\r\n"
						   "Host: {}\r\n"
						   "User-Agent: bzd\r\n"
						   "Accept: */*\r\n\r\n"_csv,
						   path,
						   hostname_);

		co_return {};
	}

private:
	Network& network_;
	Optional<typename Network::Stream> stream_;
	StringView hostname_;
	UInt32 port_;
};

} // namespace bzd
