#pragma once

#include "cc/bzd/core/async.hh"

namespace bzd {

class IO
{
public:
	virtual bzd::Async<> set() noexcept { co_return {}; }

	virtual bzd::Async<> clear() noexcept { co_return {}; }
};

} // namespace bzd
