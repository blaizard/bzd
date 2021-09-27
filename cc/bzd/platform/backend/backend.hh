#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::platform::backend {
void setOut(bzd::OStream& stream) noexcept;
void setIn(bzd::IStream& stream) noexcept;
} // namespace bzd::platform::backend
