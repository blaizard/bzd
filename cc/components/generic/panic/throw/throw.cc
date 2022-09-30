#include "cc/bzd/platform/panic.hh"

namespace bzd::platform {
void panic() { throw 42; }
} // namespace bzd::platform
