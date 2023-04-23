#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/channel.hh"

namespace bzd::platform {

/// Abort immediately the current execution of the program.
void panic(const bzd::FunctionRef<void(bzd::OStream&)> callback);

} // namespace bzd::platform
