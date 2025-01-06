#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::io {
class I2C : public bzd::IOStream
{
public:
	struct Configuration
	{
		enum class Mode
		{
			UNDEFINED = 0,
			MASTER = 1,
			SLAVE = 2
		};

		Mode mode = Mode::UNDEFINED;
		bzd::UInt8 sda = 0;
		bzd::UInt8 scl = 0;
		bool sdaPullup = false;
		bool sclPullup = false;
		bzd::UInt32 frequency = 100000;
	};
};
} // namespace bzd::io
