#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

/// Awaitable to spawn one or more executables.
class Spawn : public bzd::coroutine::impl::suspend_always
{
};

} // namespace bzd::coroutine::impl
