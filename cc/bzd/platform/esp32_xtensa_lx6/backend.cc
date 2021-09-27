#include "cc/bzd/platform/generic/stream/stub/stub.hh"
#include "cc/bzd/platform/std/stream/out/out.hh"

namespace bzd::platform::backend {
static bzd::platform::std::Out ostream;
static bzd::platform::generic::stream::Stub stub;
bzd::OStream* out = &ostream;
bzd::IStream* in = &stub;
} // namespace bzd::platform::backend
