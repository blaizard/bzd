#include "cc/bzd/core/logger/backend/logger.hh"

#include "cc/bzd/platform/stream.hh"

namespace {

class Proxy : public bzd::OStream
{
public:
	bzd::Async<bzd::Size> write(const bzd::Span<const bzd::Byte> data) noexcept override
	{
		auto size = co_await bzd::platform::out().write(data);
		co_return size;
	}
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
