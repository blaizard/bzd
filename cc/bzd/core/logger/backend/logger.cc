#include "cc/bzd/core/logger/backend/logger.hh"

namespace {

class Stub : public bzd::OStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }
};

Stub stub;
bzd::OStream* defaultBackend{&stub};
} // namespace

void bzd::backend::Logger::setDefault(bzd::OStream& backend) noexcept
{
	defaultBackend = &backend;
}

bzd::OStream& bzd::backend::Logger::getDefault() noexcept
{
	return *defaultBackend;
}
