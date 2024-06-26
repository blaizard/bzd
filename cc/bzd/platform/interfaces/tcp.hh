#pragma once

#include "cc/bdl/generator/impl/adapter/types.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/async.hh"

namespace bzd::network::tcp {

template <class Impl>
struct ClientTraits;

template <class Impl>
class Client
{
public: // Traits.
	// Type of a TCP client stream.
	using Stream = typename ClientTraits<Impl>::Stream;

public: // Methods.
	bzd::Async<Stream> connect(const StringView endpoint, const UInt16 port) noexcept
	{
		return bzd::impl::getImplementation(this)->connect(endpoint, port);
	}
};

} // namespace bzd::network::tcp
