#include "cc/bzd/core/logger/backend/logger.h"

namespace {

class LoggerStub : public bzd::backend::Logger
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }
};

LoggerStub stub;
bzd::backend::Logger* defaultBackend{&stub};
} // namespace

void bzd::backend::Logger::setDefault(bzd::backend::Logger& backend) noexcept
{
	defaultBackend = &backend;
}

bzd::backend::Logger& bzd::backend::Logger::getDefault() noexcept
{
	return *defaultBackend;
}
