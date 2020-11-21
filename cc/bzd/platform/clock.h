#pragma once

#include "bzd/platform/types.h"

namespace bzd::platform {

using ClockDurationType = bzd::UInt64Type;

bzd::ClockTickType getTicks() noexcept;
ClockDurationType msToTicks(const bzd::UInt32Type ms) noexcept;
bzd::UInt32Type ticksToMs(const bzd::ClockTickType ticks) noexcept;

} // namespace bzd::platform
