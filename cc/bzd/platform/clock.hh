#pragma once

#include "cc/bzd/core/clock.hh"

namespace bzd::platform {

/**
 * Get the steady clock for this platform.
 */
[[nodiscard]] bzd::Clock& steadyClock() noexcept;

ClockTick getTicks() noexcept;
ClockDuration msToTicks(const bzd::units::Millisecond time) noexcept;
bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept;

} // namespace bzd::platform
