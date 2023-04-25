#pragma once

#include "cc/bzd/meta/string_literal.hh"
#include "cc/components/posix/network/address/socket_type.hh"

namespace bzd::components::posix::network {

/// See /etc/protocols or https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
class Protocol
{
public:
	using Number = int;

public:
	constexpr Protocol(const SocketType type, const Number protocol = 0) noexcept : type_{type}, protocol_{protocol} {}

	/// Get the socket type associated with this protocol.
	constexpr SocketType getSocketType() const noexcept { return type_; }

	/// Get the official protocol number as it will appear within the IP header.
	constexpr Number getProtocolNumber() const noexcept { return protocol_; }

private:
	SocketType type_;
	Number protocol_;
};

namespace protocol {
inline constexpr Protocol raw{SocketType::raw, 0};
inline constexpr Protocol tcp{SocketType::stream, 6};
inline constexpr Protocol udp{SocketType::datagram, 17};
} // namespace protocol

} // namespace bzd::components::posix::network
