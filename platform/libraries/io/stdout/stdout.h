#pragma once

#include "bzd/core/io_channel.h"

#include <iostream>

namespace bzd { namespace io {
class Stdout : public bzd::OChannel
{
public:
	bzd::SizeType write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		return data.size();
	}
};
}} // namespace bzd::io
