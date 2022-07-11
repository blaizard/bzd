#include "cc/bzd/test/test.hh"
#include "cc/components/posix/network/tcp/client.hh"
#include "cc/components/posix/network/tcp/server.hh"

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
	bzd::platform::posix::network::tcp::Client client;
	co_await !client.connect("google.com", 80);

	co_return {};
}
