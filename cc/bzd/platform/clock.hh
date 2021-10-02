#pragma once

#include "cc/bzd/core/clock.hh"

namespace bzd::platform {
/**
 * Get the default steady clock for this platform.
 */
[[nodiscard]] bzd::Clock& steadyClock() noexcept;

} // namespace bzd::platform
