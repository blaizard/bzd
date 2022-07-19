#include "cc/bzd/test/test.hh"
#include "cc/components/posix/error.hh"
#include "cc/components/posix/network/tcp/client.hh"
#include "cc/components/posix/network/tcp/server.hh"
#include "cc/components/posix/proactor/sync/proactor.hh"

TEST_ASYNC(Tcp, Server)
{
	{
		bzd::platform::posix::network::tcp::Server server;
		co_await !server.bind("localhost", 8888);
		co_await !server.listen();
	}

	{
		bzd::platform::posix::network::tcp::Server server;
		co_await !server.bind("0.0.0.0", 8889);
	}

	{
		bzd::platform::posix::network::tcp::Server server;
		co_await !server.bind("::1", 8889);
	}

	co_return {};
}

TEST_ASYNC(Tcp, Client)
{
	bzd::platform::posix::sync::Proactor proactor_{};
	bzd::platform::posix::network::tcp::ClientFactory factory{proactor_};
	auto client = co_await factory.connect("localhost", 80);
	EXPECT_FALSE(client);

	// co_await !client.write("Hello"_sv.asBytes());

	co_return {};
}
