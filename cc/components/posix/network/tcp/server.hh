#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/components/posix/network/address/address.hh"
#include "cc/components/posix/network/socket.hh"

#include <iostream>

namespace bzd::components::posix::network::tcp {
class Server
{
public:
	bzd::Async<> bind(const StringView endpoint, const bzd::network::PortType port) noexcept
	{
		// Try to connect as it is an IP address
		auto maybeAddress = Address::fromIp(protocol::tcp, endpoint, port);
		if (maybeAddress)
		{
			co_await !createSocketAndBind(maybeAddress.value());
			co_return {};
		}

		// If not assume it is a hostname
		auto result = Addresses::fromHostname(protocol::tcp, endpoint, port);
		if (!result)
		{
			co_return bzd::move(result).propagate();
		}
		for (const auto& address : result.value())
		{
			if (auto socketCreated = co_await createSocketAndBind(address); !socketCreated)
			{
				// co_await !bzd::log::warning(socketCreated.error());
			}
			else
			{
				co_return {};
			}
		}
		co_return bzd::error::Failure("Server initialization failed."_csv);
	}

	bzd::Async<> listen() noexcept
	{
		if (auto result = socket_.listen(5U); !result)
		{
			co_return bzd::move(result).propagate();
		}

		co_return {};
	}

	bzd::Async<> shutdown() noexcept
	{
		socket_.reset();
		co_return {};
	}

private:
	bzd::Async<> createSocketAndBind(const Address& address) noexcept
	{
		auto maybeSocket = Socket::make(address);
		if (!maybeSocket)
		{
			co_return bzd::move(maybeSocket).propagate();
		}
		auto result = maybeSocket->bind(address);
		if (!result)
		{
			co_return bzd::move(result).propagate();
		}
		socket_ = bzd::move(maybeSocket.valueMutable());
		co_return {};
	}

	/*	void onNewConnection()
		{
			connection = accept();
			Stream stream{connection};
			handler.handle(bzd::move(stream));
		}
	*/

private:
	Socket socket_{};
};
} // namespace bzd::components::posix::network::tcp

/*
Usage in bdl:
proactor = Proactor();
webserver = Webserver();
server = Server(endpoint = "0.0.0.0:8888", proactor = proactor, handler = webserver);

Usage in C++:
class Webserver
{
public:
	void onNewConnection(auto connection)
	{
		co_await connection.read();
		co_await connection.write();
	}
};
*/
