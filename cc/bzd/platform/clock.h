#pragma once

#include "bzd/platform/types.h"

namespace bzd::platform {
bzd::ClockTickType getTicks() noexcept;
bzd::ClockTickType msToTicks(const bzd::UInt32Type ms) noexcept;
bzd::UInt32Type ticksToMs(const bzd::ClockTickType ticks) noexcept;




} // namespace bzd::platform
