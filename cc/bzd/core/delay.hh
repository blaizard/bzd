#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/units.hh"

namespace bzd {

/// Pauses the program for the amount of time (in milliseconds) specified as parameter.
///
/// \param time Time to pause the program for.
/// \return A promise object.
bzd::Async<> delay(const bzd::units::Millisecond time) noexcept;

/// Pauses the program for the amount of time (in milliseconds) specified as parameter and returns a timeout error.
///
/// \param time Time to pause the program for.
/// \return A promise object.
bzd::Async<> timeout(const bzd::units::Millisecond time) noexcept;

} // namespace bzd
