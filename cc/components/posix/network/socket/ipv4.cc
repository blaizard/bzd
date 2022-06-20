#include "cc/components/posix/network/socket/ipv4.hh"

namespace bzd::platform::posix::network {

static bzd::Result<IPv4, bzd::Error> IPv4::fromString(const StringView string, const int port) noexcept
{
	IPv4 ip{};
	const auto result = ::inet_pton(domain, string.data(), &(ip.address_.sin_addr));
	if (result == 0)
	{
		return bzd::error(ErrorType::failure, "Not a valid IPv4 {}."_csv, string);
	}
	else if (result < 0)
	{
		return bzd::error(ErrorType::failure, "inet_pton."_csv);
	}
	ip.address_.sin_family = domain;
	ip.address_.sin_port = ::htons(port);

	return ip;
}

} // namespace bzd::platform::posix::network
