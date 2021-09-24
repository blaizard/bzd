#include "cc/bzd/core/logger/backend/logger.hh"

#include "cc/bzd/core/out.hh"

namespace {

class Proxy : public bzd::OStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		const auto size = co_await bzd::Out::getDefault().write(data);
		co_return size;
	}
};

Proxy proxy;
bzd::OStream* defaultBackend{&proxy};
} // namespace

void bzd::backend::Logger::setDefault(bzd::OStream& backend) noexcept
{
	defaultBackend = &backend;
}

bzd::OStream& bzd::backend::Logger::getDefault() noexcept
{
	return *defaultBackend;
}
