#pragma once

#include "cc/components/generic/io/digital/stub/interface.hh"

namespace bzd::components::generic::io::digital {

template <class Context>
class Stub : public bzd::io::digital::Output<Stub<Context>>
{
public:
	constexpr Stub(Context&) noexcept {}
};

} // namespace bzd::components::generic::io::digital
