#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/components/posix/network/address/address.hh"
#include "cc/components/posix/network/socket.hh"
#include "cc/components/posix/network/tcp/interface.hh"
#include "cc/components/posix/proactor/proactor.hh"

namespace bzd::platform::posix::network::tcp {

template <class Config>
class Client : public bzd::platform::network::tcp::Client<Client<Config>>
{
public:
	using Stream = typename bzd::platform::network::tcp::ClientTraits<Client>::Stream;

public:
	explicit Client(Config& config) : config_{config} {}

	bzd::Async<Stream> connect(const StringView endpoint, const PortType port) noexcept
	{
		// Try to connect as it is an IP address
		auto maybeAddress = Address::fromIp(protocol::tcp, endpoint, port);
		if (maybeAddress)
		{
			auto socket = co_await !createSocketAndConnect(maybeAddress.value());
			co_return Stream(config_, bzd::move(socket));
		}

		// If not assume it is a hostname
		auto result = Addresses::fromHostname(protocol::tcp, endpoint, port);
		if (!result)
		{
			co_return bzd::move(result).propagate();
		}
		for (const auto& address : result.value())
		{
			auto maybeSocket = co_await createSocketAndConnect(address);
			if (!maybeSocket)
			{
				co_await !bzd::log::warning(maybeSocket.error());
			}
			else
			{
				co_return Stream(config_, bzd::move(maybeSocket.valueMutable()));
			}
		}
		co_return bzd::error::Failure("Client initialization failed."_csv);
	}

private:
	bzd::Async<Socket> createSocketAndConnect(const Address& address) noexcept
	{
		auto maybeSocket = Socket::make(address, SocketTypeOption::nonBlocking | SocketTypeOption::closeOnExec);
		if (!maybeSocket)
		{
			co_return bzd::move(maybeSocket).propagate();
		}
		co_await !config_.proactor.connect(maybeSocket->getFileDescriptor(), address);
		co_return bzd::move(maybeSocket.valueMutable());
	}

private:
	Config& config_;
};

} // namespace bzd::platform::posix::network::tcp

namespace bzd::platform::network::tcp {
template <class Config>
struct ClientTraits<bzd::platform::posix::network::tcp::Client<Config>>
{
	using Socket = bzd::platform::posix::network::Socket;

	class Stream : public bzd::IOStream
	{
	private:
		friend class bzd::platform::posix::network::tcp::Client<Config>;
		constexpr Stream(Config& config, Socket&& socket) noexcept : config_{config}, socket_{bzd::move(socket)} {}

	public:
		constexpr Stream(Stream&& other) noexcept : config_{other.config_}, socket_{bzd::move(other.socket_)} {}

		bzd::Async<> write(const bzd::Span<const Byte> data) noexcept override
		{
			co_return co_await config_.proactor.write(socket_.getFileDescriptor(), data);
		}

		bzd::Async<bzd::Span<const Byte>> read(bzd::Span<Byte>&& data) noexcept override
		{
			co_return co_await config_.proactor.read(socket_.getFileDescriptor(), bzd::move(data));
		}

	private:
		Config& config_;
		Socket socket_;
	};
};
} // namespace bzd::platform::network::tcp
