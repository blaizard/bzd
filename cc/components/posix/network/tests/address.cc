#include "cc/components/posix/network/address/address.hh"

#include "cc/bzd/test/test.hh"

TEST(Network, ipv4)
{
	EXPECT_TRUE(bzd::platform::posix::network::address::IpV4::fromIpPort("0.0.0.0", 32));
	EXPECT_FALSE(bzd::platform::posix::network::address::IpV4::fromIpPort("not valid ip", 32));
	EXPECT_FALSE(bzd::platform::posix::network::address::IpV4::fromIpPort("::1", 32));
}

TEST(Network, address)
{
	EXPECT_TRUE(bzd::platform::posix::network::Address::fromIpPort("0.0.0.0", 32));
	EXPECT_FALSE(bzd::platform::posix::network::Address::fromIpPort("not valid ip", 32));
	EXPECT_FALSE(bzd::platform::posix::network::Address::fromIpPort("::1", 32));
}
