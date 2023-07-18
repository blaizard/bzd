#pragma once

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

public:
	explicit Client(Context& context) noexcept : context_{context} {}

	bzd::Async<Stream> connect(const StringView hostname, const bzd::network::PortType port) noexcept
	{
		co_return Stream{context_, hostname, port};
	}

private:
	Context& context_;
};

} // namespace bzd::components::generic::network::tcp

namespace bzd::network::tcp {
template <class Context>
struct ClientTraits<bzd::components::generic::network::tcp::Client<Context>>
{
	class Stream : public bzd::IOStream
	{
	public:
		explicit constexpr Stream(Context& context, const StringView hostname, const bzd::network::PortType port) noexcept :
			context_{context}, hostname_{hostname}, port_{port}
		{
		}

	public:
		bzd::Async<> write(const bzd::Span<const Byte> data) noexcept override
		{
			struct
			{
				const bzd::Span<const Byte> data;
				const StringView hostname;
				const bzd::network::PortType port;
				const bzd::UInt32 index;
			} context{data, hostname_, port_, indexWrite_++};
			context_.config.write(context);
			co_return {};
		}

		bzd::Async<bzd::Span<const Byte>> read(bzd::Span<Byte>&& data) noexcept override
		{
			struct
			{
				const bzd::Span<Byte> data;
				const StringView hostname;
				const bzd::network::PortType port;
				const bzd::UInt32 index;
			} context{bzd::move(data), hostname_, port_, indexRead_++};
			const auto output = context_.config.read(bzd::move(context));
			co_return output;
		}

	private:
		Context& context_;
		StringView hostname_;
		bzd::network::PortType port_;
		bzd::UInt32 indexRead_{0u};
		bzd::UInt32 indexWrite_{0u};
	};
};
} // namespace bzd::network::tcp
