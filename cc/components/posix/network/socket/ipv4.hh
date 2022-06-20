#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/error.hh"

#include <sys/socket.h>
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
	static bzd::Result<IPv4, bzd::Error> fromString(const StringView string, const int port) noexcept;

	::sockaddr* address() noexcept { return static_cast<::sockaddr*>(&address_); }

	constexpr ::socklen_t size() const noexcept { return sizeof(address_); }

private:
	::sockaddr_in address_{};
};
} // namespace bzd::platform::posix::network
