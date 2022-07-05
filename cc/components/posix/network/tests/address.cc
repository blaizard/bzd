#include "cc/components/posix/network/address/address.hh"

#include "cc/bzd/test/test.hh"

namespace bzd::platform::posix::network {

TEST(Address, fromIpV4)
{
	EXPECT_TRUE(Address::fromIpV4(protocol::udp, "0.0.0.0", 32));
	EXPECT_FALSE(Address::fromIpV4(protocol::udp, "not valid ip", 32));
	EXPECT_FALSE(Address::fromIpV4(protocol::udp, "::1", 32));
}

TEST(Address, fromIp)
{
	EXPECT_TRUE(Address::fromIp(protocol::udp, "0.0.0.0", 32));
	EXPECT_FALSE(Address::fromIp(protocol::udp, "not valid ip", 32));
	EXPECT_FALSE(Address::fromIp(protocol::udp, "::1", 32));
}

TEST(Address, Addresses)
{
	const auto result = Addresses::fromHostname(protocol::udp, "localhost", 80);
	EXPECT_TRUE(result);
	for ([[maybe_unused]] const auto& address : result.value())
	{
	}
}

} // namespace bzd::platform::posix::network
