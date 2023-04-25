#pragma once

#include <sys/socket.h>

namespace bzd::components::posix::network {

enum class AddressFamily
{
	/// IPv4 Internet protocols.
	ipV4 = AF_INET,
	/// IPv6 Internet protocols.
	ipV6 = AF_INET6
};

}
