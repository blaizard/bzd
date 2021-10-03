#include "cc/bzd/platform/esp32_xtensa_lx6/xthal_clock/xthal_clock.hh"
#include "cc/bzd/platform/generic/clock/null/null.hh"
#include "cc/bzd/platform/generic/stream/stub/stub.hh"
#include "cc/bzd/platform/std/stream/out/out.hh"

namespace bzd::platform::backend {

static bzd::platform::std::Out implOut;
static bzd::platform::generic::stream::Stub stub;
static bzd::platform::generic::clock::Null nullClock;
static bzd::platform::esp32::XthalClock implXthalClock;

bzd::OStream* out = &implOut;
bzd::IStream* in = &stub;
bzd::Clock* steadyClock = &implXthalClock;
bzd::Clock* systemClock = &nullClock;

} // namespace bzd::platform::backend
