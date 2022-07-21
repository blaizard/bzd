#pragma once

#include "cc/bzd/container/string_view.hh"
#include "tools/bdl/generators/cc/adapter/types.hh"

namespace bzd::platform::network::tcp {

template <class Impl>
class Client
{
public:
	auto connect(const StringView endpoint, const UInt16 port) noexcept
	{
		return bzd::impl::getImplementation(this)->connect(endpoint, port);
	}
};

} // namespace bzd::platform::network::tcp
