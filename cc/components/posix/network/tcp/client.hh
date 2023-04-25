#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/components/posix/network/address/address.hh"
#include "cc/components/posix/network/socket.hh"
#include "cc/components/posix/network/tcp/interface.hh"
#include "cc/components/posix/proactor/proactor.hh"

namespace bzd::components::posix::network::tcp {

template <class Context>
class Client : public bzd::network::tcp::Client<Client<Context>>
{
public:
	using Stream = typename bzd::network::tcp::ClientTraits<Client>::Stream;

public:
	explicit Client(Context& context) : context_{context}, logger_{context.config.out} {}

	bzd::Async<Stream> connect(const StringView endpoint, const PortType port) noexcept
	{
		// Try to connect as it is an IP address
		auto maybeAddress = Address::fromIp(protocol::tcp, endpoint, port);
		if (maybeAddress)
		{
			auto socket = co_await !createSocketAndConnect(maybeAddress.value());
			co_return Stream(context_, bzd::move(socket));
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
				co_await !logger_.warning(maybeSocket.error());
			}
			else
			{
				co_return Stream(context_, bzd::move(maybeSocket.valueMutable()));
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
		co_await !context_.config.proactor.connect(maybeSocket->getFileDescriptor(), address);
		co_return bzd::move(maybeSocket.valueMutable());
	}

private:
	Context& context_;
	bzd::Logger logger_;
};

} // namespace bzd::components::posix::network::tcp

namespace bzd::network::tcp {
template <class Context>
struct ClientTraits<bzd::components::posix::network::tcp::Client<Context>>
{
	using Socket = bzd::components::posix::network::Socket;

	class Stream : public bzd::IOStream
	{
	private:
		friend class bzd::components::posix::network::tcp::Client<Context>;
		constexpr Stream(Context& context, Socket&& socket) noexcept : context_{context}, socket_{bzd::move(socket)} {}

	public:
		constexpr Stream(Stream&& other) noexcept : context_{other.context_}, socket_{bzd::move(other.socket_)} {}

		bzd::Async<> write(const bzd::Span<const Byte> data) noexcept override
		{
			co_return co_await context_.config.proactor.write(socket_.getFileDescriptor(), data);
		}

		bzd::Async<bzd::Span<const Byte>> read(bzd::Span<Byte>&& data) noexcept override
		{
			co_return co_await context_.config.proactor.read(socket_.getFileDescriptor(), bzd::move(data));
		}

	private:
		Context& context_;
		Socket socket_;
	};
};
} // namespace bzd::network::tcp
