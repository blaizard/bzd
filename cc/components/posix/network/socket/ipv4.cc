#include "cc/components/posix/network/socket/ipv4.hh"

#include "cc/bzd/core/error.hh"
#include "cc/components/posix/error.hh"

#include <arpa/inet.h>
#include <sys/socket.h>

namespace bzd::platform::posix::network {

bzd::Result<IPv4, bzd::Error> IPv4::fromString(const StringView string, const int port) noexcept
{
	IPv4 ip{};
	const auto result = ::inet_pton(domain, string.data(), &(ip.address_.sin_addr));
	if (result == 0)
	{
		return bzd::error::Failure{"Not a valid IP v4 address: '{}'."_csv, string};
	}
	else if (result < 0)
	{
		return bzd::error::Posix("inet_pton");
	}
	ip.address_.sin_family = domain;
	ip.address_.sin_port = ::htons(port);

	return ip;
}

} // namespace bzd::platform::posix::network
