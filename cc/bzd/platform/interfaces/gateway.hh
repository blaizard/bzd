#pragma once

#include "cc/bdl/generator/impl/adapter/types.hh"
#include "cc/bzd/core/async.hh"

namespace bzd {

template <class Impl>
struct GatewayTraits;

template <class Impl>
class Gateway
{
public: // Traits.
	// Type of a gateway stream.
	using Stream = typename GatewayTraits<Impl>::Stream;
	using Config = typename GatewayTraits<Impl>::Config;

public: // Methods.
		// Connect to another gateway.
		// The argument passed is the config used to create the other gateway.
	bzd::Async<Stream> connect(const Config& config) noexcept { return bzd::impl::getImplementation(this)->connect(config); }
};

} // namespace bzd
