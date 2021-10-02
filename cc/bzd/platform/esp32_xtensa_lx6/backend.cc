#include "cc/bzd/platform/generic/stream/stub/stub.hh"
#include "cc/bzd/platform/std/stream/out/out.hh"
#include "cc/bzd/platform/esp32_xtensa_lx6/xthal_clock/xthal_clock.hh"

namespace bzd::platform::backend {

static bzd::platform::std::Out implOut;
static bzd::platform::generic::stream::Stub stub;
static bzd::platform::esp32::XthalClock implXthalClock;

bzd::OStream* out = &implOut;
bzd::IStream* in = &stub;
bzd::Clock* steadyClock = &implXthalClock;

} // namespace bzd::platform::backend
