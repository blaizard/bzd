#include "cc/components/generic/clock/system/null/null.hh"

namespace bzd::platform::generic::clock::system {

bzd::Result<bzd::units::Millisecond, bzd::Error> Null::getTime() noexcept { return static_cast<bzd::units::Millisecond>(0); }

} // namespace bzd::platform::generic::clock::system
