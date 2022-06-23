#include "cc/components/posix/network/address/address.hh"

#include "cc/bzd/core/error.hh"

#include <sys/socket.h>
#include <sys/types.h>

namespace bzd::platform::posix::network {

bzd::Result<Address, bzd::Error> Address::fromIpPort(const StringView string, const PortType port) noexcept
{
	// Choose between ipv4 and ipv6.
	auto maybeIpV4 = address::IpV4::fromIpPort(string, port);
	if (!maybeIpV4)
	{
		return bzd::move(maybeIpV4).propagate();
	}
	return Address{bzd::move(maybeIpV4).valueMutable()};
}

bzd::Result<Addresses, bzd::Error> Addresses::fromHostnamePort(const StringView hostname, const PortType port) noexcept
{
	Addresses addresses{};
	String<8> portStr;
	toString(portStr, port);

	if (const auto result = ::getaddrinfo(hostname.data(), portStr.data(), nullptr, &addresses.addrs_); result != 0)
	{
		return bzd::error::Failure("getaddrinfo for {}:{}, {}"_csv, hostname, portStr, ::gai_strerror(result));
	}
	return addresses;
}

Addresses::~Addresses() noexcept
{
	if (addrs_)
	{
		::freeaddrinfo(addrs_);
		addrs_ = nullptr;
	}
}

} // namespace bzd::platform::posix::network
