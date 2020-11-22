#pragma once

#include "bzd/core/units.h"
#include "bzd/platform/types.h"

namespace bzd::platform {

using ClockDurationType = bzd::UInt64Type;

bzd::ClockTickType getTicks() noexcept;
ClockDurationType msToTicks(const bzd::units::Millisecond time) noexcept;
bzd::units::Millisecond ticksToMs(const bzd::ClockTickType ticks) noexcept;

} // namespace bzd::platform
