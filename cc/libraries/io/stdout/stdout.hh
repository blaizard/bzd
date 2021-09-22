#pragma once

#include "cc/bzd/core/channel.hh"

#include <iostream>

namespace bzd::io {
class Stdout : public bzd::OStream
{
public:
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		return data.size();
	}
};
} // namespace bzd::io
