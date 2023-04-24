#include "cc/bzd/test/test.hh"
#include "cc/components/posix/error.hh"
#include "cc/components/posix/network/tcp/client.hh"
#include "cc/components/posix/network/tcp/server.hh"
#include "cc/components/posix/proactor/mock/proactor.hh"
#include "cc/components/posix/proactor/sync/proactor.hh"

TEST_ASYNC(Tcp, Server)
{
	// TODO: use an API abstraction.
	/*
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
	*/

	co_return {};
}

TEST_ASYNC(Tcp, Client)
{
	struct Config
	{
		bzd::platform::posix::sync::Proactor proactor{};
	};
	struct Context
	{
		Config config{};
	};
	/*Context context{};
	bzd::platform::posix::network::tcp::Client client{context};
	[[maybe_unused]] auto stream = co_await client.connect("google.com", 80);*/
	co_return {};
}
