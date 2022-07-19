#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/components/posix/network/address/address.hh"
#include "cc/components/posix/network/socket.hh"
#include "cc/components/posix/proactor/proactor.hh"

namespace bzd::platform::posix::network::tcp {

template <class Proactor>
class ClientFactory
{
public:
	class Stream : public bzd::OStream
	{
	private:
		friend class ClientFactory<Proactor>;
		constexpr Stream(Proactor& proactor, Socket&& socket) noexcept : proactor_{proactor}, socket_{bzd::move(socket)} {}

	public:
		constexpr Stream(Stream&& other) noexcept : proactor_{other.proactor_}, socket_{bzd::move(other.socket_)} {}

		bzd::Async<Size> write(const bzd::Span<const Byte> data) noexcept override
		{
			co_return co_await proactor_.write(socket_.getFileDescriptor(), data);
		}

		/*bzd::Async<bzd::Span<Byte>> read(const bzd::Span<Byte> data) noexcept override
		{
			co_return co_await proactor_.read(socket_.getFileDescriptor(), data);
		}*/

	private:
		Proactor& proactor_;
		Socket socket_;
	};

public:
	explicit ClientFactory(Proactor& proactor) : proactor_{proactor} {}

	bzd::Async<Stream> connect(const StringView endpoint, const PortType port) noexcept
	{
		// Try to connect as it is an IP address
		auto maybeAddress = Address::fromIp(protocol::tcp, endpoint, port);
		if (maybeAddress)
		{
			auto socket = co_await !createSocketAndConnect(maybeAddress.value());
			co_return Stream(proactor_, bzd::move(socket));
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
				co_return Stream(proactor_, bzd::move(maybeSocket.valueMutable()));
			}
		}
		co_return bzd::error::Failure("Client initialization failed."_csv);
	}

private:
	bzd::Async<Socket> createSocketAndConnect(const Address& address) noexcept
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
		co_return bzd::move(maybeSocket.valueMutable());
	}

private:
	Proactor& proactor_;
};

} // namespace bzd::platform::posix::network::tcp
