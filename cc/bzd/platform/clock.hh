#pragma once

#include "cc/bzd/platform/interfaces/clock.hh"

namespace bzd::platform {
/**
 * Get the default steady clock for this platform.
 * This is a monotonic clock.
 */
[[nodiscard]] bzd::SteadyClock& steadyClock() noexcept;

/**
 * Get the default system clock for this platform.
 * This represents the best guest of the actual UTC time.
 */
[[nodiscard]] bzd::SystemClock& systemClock() noexcept;

} // namespace bzd::platform
