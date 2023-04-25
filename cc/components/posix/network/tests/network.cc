#include "cc/components/posix/network/network.hh"

#include "cc/bzd/test/test.hh"

TEST_ASYNC(Network, connect)
{
	/*	bzd::components::posix::network::TCP tcp{};
		const auto result = co_await tcp.connect("localhost", 80);
		EXPECT_ASYNC_TRUE(result);
	*/
	co_return {};
}
