#include "cc/components/generic/timer/null/null.hh"

namespace bzd::components::generic::timer {

bzd::Result<bzd::units::Millisecond, bzd::Error> Null::getTime() noexcept { return static_cast<bzd::units::Millisecond>(0); }

} // namespace bzd::components::generic::timer
