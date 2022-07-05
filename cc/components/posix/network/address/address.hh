#pragma once

#include "cc/bzd/container/variant.hh"
#include "cc/components/posix/network/address/family.hh"
#include "cc/components/posix/network/address/protocol.hh"
#include "cc/components/posix/network/address/socket_type.hh"
#include "cc/components/posix/network/types.hh"

#include <netdb.h>

namespace bzd::platform::posix::network {

// Address family abstraction.
class Address
{
public:
	/// Create an address object from an IP v4 address and a port number.
	[[nodiscard]] static bzd::Result<Address, bzd::Error> fromIpV4(const Protocol protocol,
																   const StringView string,
																   const PortType port) noexcept;

	/// Create an address object from an IP address and a port number.
	[[nodiscard]] static bzd::Result<Address, bzd::Error> fromIp(const Protocol protocol,
																 const StringView string,
																 const PortType port) noexcept;

	constexpr AddressFamily family() const noexcept { return AddressFamily{storage_.ss_family}; }
	constexpr const ::sockaddr* native() noexcept { return &storageErased_; }
	constexpr ::socklen_t size() const noexcept { return size_; }

private:
	constexpr Address(const Size size, const Protocol protocol) noexcept : size_{size}, type_{protocol.getSocketType()}, protocol_{protocol.getProtocolNumber()} {}

	/// Create an address object from an addrinfo structure.
	constexpr Address(const ::addrinfo&) noexcept {}

private:
	// Use an union here to avoid using reinterpret_cast, this will prevent the usage of constexpr.
	union {
		::sockaddr storageErased_;
		::sockaddr_storage storage_;
	};
	Size size_;
	SocketType type_;
	Protocol::Number protocol_;
};

// List of addresses.
class Addresses
{
public:
	/// Create an address object from a hostname and a port number.
	[[nodiscard]] static bzd::Result<Addresses, bzd::Error> fromHostname(const Protocol protocol,
																		 const StringView hostname,
																		 const PortType port) noexcept;

	~Addresses() noexcept;
	/*
		begin() noexcept;

		end() noexcept;
	*/
private:
	constexpr Addresses() noexcept = default;

private:
	::addrinfo* addrs_{nullptr};
};

} // namespace bzd::platform::posix::network
