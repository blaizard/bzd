#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::platform {

/// Get the default output stream for this platform.
[[nodiscard]] bzd::OStream& out() noexcept;

/// Get the default input stream for this platform.
[[nodiscard]] bzd::IStream& in() noexcept;

} // namespace bzd::platform
