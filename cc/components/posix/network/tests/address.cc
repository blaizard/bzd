#include "cc/components/posix/network/address/address.hh"

#include "cc/bzd/test/test.hh"

namespace bzd::platform::posix::network {

TEST(Network, ipv4)
{
	EXPECT_TRUE(Address::fromIpV4(protocol::udp, "0.0.0.0", 32));
	EXPECT_FALSE(Address::fromIpV4(protocol::udp, "not valid ip", 32));
	EXPECT_FALSE(Address::fromIpV4(protocol::udp, "::1", 32));
}

TEST(Network, address)
{
	EXPECT_TRUE(Address::fromIp(protocol::udp, "0.0.0.0", 32));
	EXPECT_FALSE(Address::fromIp(protocol::udp, "not valid ip", 32));
	EXPECT_FALSE(Address::fromIp(protocol::udp, "::1", 32));
}

} // namespace bzd::platform::posix::network
