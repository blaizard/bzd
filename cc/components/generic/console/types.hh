#pragma once

#include "cc/bzd.hh"

namespace bzd::components::generic {

using Action = bzd::FunctionRef<bzd::Async<>(void)>;
using Output = bzd::Variant<Action, void*>;
using Map = bzd::interface::Map<bzd::StringView, Output>;

} // namespace bzd::components::generic
