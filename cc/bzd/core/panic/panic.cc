#include "cc/bzd/core/panic.hh"

namespace {
void panic() { throw 42; }
bzd::Panic::Type defaultPanic{panic};
} // namespace

void bzd::Panic::setDefault(bzd::Panic::Type panic) noexcept { defaultPanic = panic; }

void bzd::Panic::trigger() { defaultPanic(); }
