#include "cc/bzd/core/logger/backend/logger.h"

namespace {
bzd::backend::Logger* defaultBackend{nullptr};
}

void bzd::backend::Logger::setDefault(bzd::backend::Logger& backend) noexcept
{
	defaultBackend = &backend;
}

bzd::backend::Logger& bzd::backend::Logger::getDefaultBackend() noexcept
{
	return *defaultBackend;
}
