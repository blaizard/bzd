#include "cc/bzd/platform/backend/backend.hh"

#include "cc/bzd/platform/clock.hh"
#include "cc/bzd/platform/stream.hh"

namespace bzd::platform::backend {
extern bzd::OStream* out;
extern bzd::IStream* in;
extern bzd::Clock* steadyClock;
extern bzd::Clock* systemClock;
} // namespace bzd::platform::backend

bzd::OStream& bzd::platform::out() noexcept
{
	return *bzd::platform::backend::out;
}

bzd::IStream& bzd::platform::in() noexcept
{
	return *bzd::platform::backend::in;
}

bzd::Clock& bzd::platform::steadyClock() noexcept
{
	return *bzd::platform::backend::steadyClock;
}

bzd::Clock& bzd::platform::systemClock() noexcept
{
	return *bzd::platform::backend::systemClock;
}
