#pragma once

#include "cc/bzd/container/variant.hh"
#include "cc/components/posix/network/address/family.hh"
#include "cc/components/posix/network/address/ipv4.hh"
#include "cc/components/posix/network/types.hh"

#include <netdb.h>

namespace bzd::platform::posix::network {

// Address family abstraction.
class Address
{
public:
	/// Create an address object from an IP address and a port number.
	[[nodiscard]] static bzd::Result<Address, bzd::Error> fromIpPort(const StringView string, const PortType port) noexcept;

	constexpr AddressFamily family() const noexcept
	{
		return impl_.match([](const auto& impl) { return impl.family; });
	}
	constexpr ::sockaddr* address() noexcept
	{
		return impl_.match([](auto& impl) { return impl.address(); });
	}
	constexpr ::socklen_t size() const noexcept
	{
		return impl_.match([](const auto& impl) { return impl.size(); });
	}

private:
	template <class Impl>
	constexpr explicit Address(Impl&& impl) noexcept : impl_{bzd::forward<Impl>(impl)}
	{
	}

private:
	bzd::Variant<address::IpV4> impl_;
};

// List of addresses.
class Addresses
{
public:
	/// Create an address object from a hostname and a port number.
	[[nodiscard]] static bzd::Result<Addresses, bzd::Error> fromHostnamePort(const StringView hostname, const PortType port) noexcept;

	~Addresses() noexcept;

private:
	constexpr Addresses() noexcept = default;

private:
	::addrinfo* addrs_{nullptr};
};

} // namespace bzd::platform::posix::network
