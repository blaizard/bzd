#include "cc/bzd/platform/backend/backend.hh"

#include "cc/bzd/platform/stream.hh"

namespace bzd::platform::backend {
extern bzd::OStream* out;
extern bzd::IStream* in;
} // namespace bzd::platform::backend

bzd::OStream& bzd::platform::out() noexcept
{
	return *bzd::platform::backend::out;
}

bzd::IStream& bzd::platform::in() noexcept
{
	return *bzd::platform::backend::in;
}

void bzd::platform::backend::setOut(bzd::OStream& stream) noexcept
{
	bzd::platform::backend::out = &stream;
}

void bzd::platform::backend::setIn(bzd::IStream& stream) noexcept
{
	bzd::platform::backend::in = &stream;
}
