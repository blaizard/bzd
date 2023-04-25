#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::async::awaitable {

/// Awaitable to spawn one or more executables.
class Spawn : public bzd::async::impl::suspend_always
{
};

} // namespace bzd::async::awaitable
