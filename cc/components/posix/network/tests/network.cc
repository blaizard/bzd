#include "cc/components/posix/network/network.hh"

#include "cc/bzd/test/test.hh"

TEST_ASYNC(Network, connect)
{
	bzd::platform::posix::network::TCP tcp{};

	const auto result = co_await tcp.connect("www.google.com", 80);
	EXPECT_ASYNC_TRUE(result);

	co_return {};
}
