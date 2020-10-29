#pragma once

#include "bzd/core/channel.h"

namespace bzd::io {
class I2C : public bzd::IOChannel
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
		bzd::UInt8Type sda = 0;
		bzd::UInt8Type scl = 0;
		bool sdaPullup = false;
		bool sclPullup = false;
		bzd::UInt32Type frequency = 100000;
	};
};
} // namespace bzd::io
