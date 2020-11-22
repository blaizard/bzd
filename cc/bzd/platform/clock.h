#pragma once

#include "bzd/container/named_type.h"
#include "bzd/core/units.h"
#include "bzd/platform/types.h"

namespace bzd::platform {

using ClockTick = bzd::NamedType<bzd::UInt32Type, struct ClockTickTag, bzd::Arithmetic>;
using ClockDuration = bzd::NamedType<bzd::UInt64Type, struct ClockDurationTag, bzd::Arithmetic>;

ClockTick getTicks() noexcept;
ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept;
bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept;

} // namespace bzd::platform
