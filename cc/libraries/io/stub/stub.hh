#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::io {
class Stub : public bzd::OStream
{
public:
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override { return data.size(); }
};
} // namespace bzd::io
