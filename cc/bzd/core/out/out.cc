#include "cc/bzd/core/out.hh"

#include "cc/bzd/platform/generic/stream/stub/stub.hh"

namespace {
bzd::platform::generic::stream::Stub stub;
bzd::OStream* defaultOut{&stub};
} // namespace

void bzd::Out::setDefault(bzd::OStream& out) noexcept
{
	defaultOut = &out;
}

bzd::OStream& bzd::Out::getDefault() noexcept
{
	return *defaultOut;
}
