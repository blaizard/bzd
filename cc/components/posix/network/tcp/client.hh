#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/components/posix/network/address/address.hh"
#include "cc/components/posix/network/socket.hh"

namespace bzd::platform::posix::network::tcp {

class Client //: public bzd::IOStream
{
public:
	bzd::Async<> connect(const StringView endpoint, const PortType port) noexcept
	{
		// Try to connect as it is an IP address
		auto maybeAddress = Address::fromIp(protocol::tcp, endpoint, port);
		if (maybeAddress)
		{
			co_await !createSocketAndConnect(maybeAddress.value());
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
			if (auto socketCreated = co_await createSocketAndConnect(address); !socketCreated)
			{
				co_await !bzd::log::warning(socketCreated.error());
			}
			else
			{
				co_return {};
			}
		}
		co_return bzd::error::Failure("Client initialization failed."_csv);
	}
	/*
		bzd::Async<Size> write(const bzd::Span<const Byte> data) noexcept override
		{

		}

		bzd::Async<bzd::Span<Byte>> read(const bzd::Span<Byte> data) noexcept override
		{

		}
	*/
private:
	bzd::Async<> createSocketAndConnect(const Address& address) noexcept
	{
		auto maybeSocket = Socket::make(address);
		if (!maybeSocket)
		{
			co_return bzd::move(maybeSocket).propagate();
		}
		auto result = maybeSocket->connect(address);
		if (!result)
		{
			co_return bzd::move(result).propagate();
		}
		socket_ = bzd::move(maybeSocket.valueMutable());
		co_return {};
	}

private:
	Socket socket_{};
};

} // namespace bzd::platform::posix::network::tcp
