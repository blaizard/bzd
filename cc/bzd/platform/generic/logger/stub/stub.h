#pragma once

#include "cc/bzd/core/logger/backend/logger.h"

namespace bzd::platform::generic {
class LoggerStub : public bzd::backend::Logger
{
public:
	bzd::Async<SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }
};
} // namespace bzd::platform::generic
