#include "cc/bzd/platform/generic/stream/stub/stub.hh"
#include "cc/bzd/platform/std/stream/out/out.hh"
#include "cc/bzd/platform/std/steady_clock/steady_clock.hh"

namespace bzd::platform::backend {
static bzd::platform::std::Out implOut;
static bzd::platform::generic::stream::Stub stub;
static bzd::platform::std::SteadyClock implSteadyClock;
bzd::OStream* out = &implOut;
bzd::IStream* in = &stub;
bzd::Clock* steadyClock = &implSteadyClock;
} // namespace bzd::platform::backend
