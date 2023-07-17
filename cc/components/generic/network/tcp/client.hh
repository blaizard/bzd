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

	bzd::Async<Stream> connect(const StringView, const bzd::network::PortType) noexcept { co_return Stream{context_}; }

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
		explicit constexpr Stream(Context& context) noexcept : context_{context} {}

	public:
		bzd::Async<> write(const bzd::Span<const Byte>) noexcept override { co_return {}; }

		bzd::Async<bzd::Span<const Byte>> read(bzd::Span<Byte>&& data) noexcept override
		{
			const auto output = context_.config.read(bzd::move(data));
			co_return output;
		}

	private:
		Context& context_;
		Size countRead_{0u};
		Size countWrite_{0u};
	};
};
} // namespace bzd::network::tcp
