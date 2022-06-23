#include "cc/components/posix/network/address/ipv4.hh"

#include "cc/bzd/core/error.hh"
#include "cc/bzd/utility/to_underlying.hh"
#include "cc/components/posix/error.hh"
#include "cc/components/posix/network/address/address.hh"

#include <arpa/inet.h>
#include <sys/socket.h>

namespace bzd::platform::posix::network::address {

bzd::Result<IpV4, bzd::Error> IpV4::fromIpPort(const StringView string, const int port) noexcept
{
	IpV4 ip{};
	const auto result = ::inet_pton(bzd::toUnderlying(family), string.data(), &(ip.address_.sin_addr));
	if (result == 0)
	{
		return bzd::error::Failure{"Not a valid IP v4 address: '{}'."_csv, string};
	}
	else if (result < 0)
	{
		return bzd::error::Posix("inet_pton");
	}
	ip.address_.sin_family = bzd::toUnderlying(family);
	ip.address_.sin_port = ::htons(port);

	return ip;
}

} // namespace bzd::platform::posix::network::address
