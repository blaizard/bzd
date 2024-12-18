#pragma once

#include "cc/bzd/core/assert.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/components/generic/network/tcp/interface.hh"

namespace bzd::components::generic::network::tcp {

template <class Context>
class Client
{
public:
	using Stream = typename bzd::network::tcp::ClientTraits<Client>::Stream;
	using Metadata = typename bzd::network::tcp::ClientTraits<Client>::Metadata;

public:
	virtual ~Client() = default;

	bzd::Async<Stream> connect(const StringView hostname, const bzd::network::PortType port) noexcept
	{
		co_return Stream{*this, hostname, port};
	}

	virtual bzd::Span<const bzd::Byte> read(bzd::Span<Byte>&&, Metadata&) noexcept { return {}; }

	virtual void write(const bzd::Span<const Byte>, Metadata&) noexcept {}
};

} // namespace bzd::components::generic::network::tcp

namespace bzd::network::tcp {
template <class Context>
struct ClientTraits<bzd::components::generic::network::tcp::Client<Context>>
{
	struct Metadata
	{
		const StringView hostname;
		const bzd::network::PortType port;
		bzd::UInt32& index;
	};

	class Stream : public bzd::IOStream
	{
	public:
		explicit constexpr Stream(bzd::components::generic::network::tcp::Client<Context>& client,
								  const StringView hostname,
								  const bzd::network::PortType port) noexcept :
			client_{client},
			hostname_{hostname}, port_{port}
		{
		}

	public:
		bzd::Async<> write(const bzd::Span<const Byte> data) noexcept override
		{
			co_await bzd::async::yield();
			Metadata metadata{hostname_, port_, indexWrite_};
			client_.write(data, metadata);
			co_return {};
		}

		bzd::Async<bzd::Span<const Byte>> read(bzd::Span<Byte>&& data) noexcept override
		{
			co_await bzd::async::yield();
			const auto maximumSize = data.size();
			Metadata metadata{hostname_, port_, indexRead_};
			const auto output = client_.read(bzd::move(data), metadata);
			bzd::assert::isTrue(output.size() <= maximumSize,
								"config.read() returned too much data: {} vs {}"_csv,
								output.size(),
								maximumSize);
			co_return output;
		}

	private:
		bzd::components::generic::network::tcp::Client<Context>& client_;
		StringView hostname_;
		bzd::network::PortType port_;
		bzd::UInt32 indexRead_{0u};
		bzd::UInt32 indexWrite_{0u};
	};
};
} // namespace bzd::network::tcp
