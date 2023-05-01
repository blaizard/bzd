#pragma once

#include "cc/bzd/meta/type.hh"

namespace bzd::meta {

template <class... Ts>
struct Last
{
	using Type = typename decltype((bzd::meta::Type<Ts>{}, ...))::Type;
};

} // namespace bzd::meta
