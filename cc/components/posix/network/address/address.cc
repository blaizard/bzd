#include "cc/components/posix/network/address/address.hh"

#include "cc/bzd/algorithm/fill.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/utility/pattern/to_string.hh"
#include "cc/components/posix/error.hh"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace bzd::components::posix::network {

bzd::Result<Address, bzd::Error> Address::fromIpV4(const Protocol protocol,
												   const StringView string,
												   const bzd::network::PortType port) noexcept
{
	Address address{sizeof(::sockaddr_in), protocol};
	::sockaddr_in& storage{reinterpret_cast<::sockaddr_in&>(address.storage_)};

	::memset(&storage, 0, sizeof(storage));
	const auto result = ::inet_pton(AF_INET, string.data(), &(storage.sin_addr));
	if (result == 0)
	{
		return bzd::error::Failure{"Not a valid IP v4 address: '{}'."_csv, string};
	}
	else if (result < 0)
	{
		return bzd::error::Errno("inet_pton");
	}
	storage.sin_family = AF_INET;
	storage.sin_port = ::htons(port);

	return address;
}

bzd::Result<Address, bzd::Error> Address::fromIpV6(const Protocol protocol,
												   const StringView string,
												   const bzd::network::PortType port) noexcept
{
	Address address{sizeof(::sockaddr_in6), protocol};
	::sockaddr_in6& storage{reinterpret_cast<::sockaddr_in6&>(address.storage_)};

	::memset(&storage, 0, sizeof(storage));
	const auto result = ::inet_pton(AF_INET6, string.data(), &(storage.sin6_addr));
	if (result == 0)
	{
		return bzd::error::Failure{"Not a valid IP v6 address: '{}'."_csv, string};
	}
	else if (result < 0)
	{
		return bzd::error::Errno("inet_pton");
	}
	storage.sin6_family = AF_INET6;
	storage.sin6_port = ::htons(port);

	return address;
}

bzd::Result<Address, bzd::Error> Address::fromIp(const Protocol protocol,
												 const StringView string,
												 const bzd::network::PortType port) noexcept
{
	// Choose between ipv4 and ipv6.
	return (string.contains(':')) ? fromIpV6(protocol, string, port) : fromIpV4(protocol, string, port);
}

bzd::Result<Addresses, bzd::Error> Addresses::fromHostname(const Protocol protocol,
														   const StringView hostname,
														   const bzd::network::PortType port) noexcept
{
	Addresses addresses{};
	::addrinfo hints{};
	String<8> portStr;
	bzd::toString(portStr.assigner(), port);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = bzd::toUnderlying(protocol.getSocketType());
	hints.ai_protocol = protocol.getProtocolNumber();

	if (const auto result = ::getaddrinfo(hostname.data(), portStr.data(), &hints, &addresses.addr_); result != 0)
	{
		return bzd::error::Failure("'{}:{}', {}"_csv, hostname, portStr, ::gai_strerror(result));
	}
	return addresses;
}

void Addresses::reset() noexcept
{
	if (addr_)
	{
		::freeaddrinfo(addr_);
		addr_ = nullptr;
	}
}

Addresses::~Addresses() noexcept { reset(); }

} // namespace bzd::components::posix::network
