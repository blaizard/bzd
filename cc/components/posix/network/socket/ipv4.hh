#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/error.hh"

#include <netinet/in.h>

namespace bzd::platform::posix::network {
// Socket address information.
class IPv4
{
public:
	static constexpr int domain{AF_INET};

private:
	constexpr IPv4() noexcept = default;

public:
	/// Create an IPv4 object from a string.
	static bzd::Result<IPv4, bzd::Error> fromString(const StringView string, const int port) noexcept;

	/// Get the erased native address of this object.
	constexpr ::sockaddr* address() noexcept { return &addressErased_; }

	/// Get the length of the address.
	constexpr ::socklen_t size() const noexcept { return sizeof(address_); }

private:
	// Use an union here to avoid using reinterpret_cast, this will prevent the usage of constexpr.
	union {
		::sockaddr addressErased_;
		::sockaddr_in address_;
	};
};
} // namespace bzd::platform::posix::network
