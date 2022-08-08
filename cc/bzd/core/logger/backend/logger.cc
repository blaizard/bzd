#include "cc/bzd/core/logger/backend/logger.hh"

#include "cc/bzd/platform/stream.hh"

namespace {

class Proxy : public bzd::OStream
{
public:
	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept override { co_return co_await !bzd::platform::out().write(data); }
};

Proxy proxy;
bzd::OStream* defaultBackend{&proxy};
} // namespace

bzd::Optional<bzd::OStream&> bzd::backend::Logger::setDefault(bzd::OStream& backend) noexcept
{
	auto previous = defaultBackend;
	defaultBackend = &backend;
	if (previous == &::proxy)
	{
		return bzd::nullopt;
	}
	return *previous;
}

bzd::OStream& bzd::backend::Logger::getDefault() noexcept
{
	return *defaultBackend;
}
