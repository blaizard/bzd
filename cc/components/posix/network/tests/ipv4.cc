#include "cc/components/posix/network/socket/ipv4.hh"

#include "cc/bzd/test/test.hh"

TEST_ASYNC(Network, ipv4)
{
	EXPECT_TRUE(bzd::platform::posix::network::IPv4::fromString("0.0.0.0", 32));
	EXPECT_FALSE(bzd::platform::posix::network::IPv4::fromString("not valid ip", 32));
	EXPECT_FALSE(bzd::platform::posix::network::IPv4::fromString("::1", 32));
	co_return {};
}
