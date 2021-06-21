#pragma once

#include "cc/bzd/platform/core.h"
#include "cc/bzd/core/log.h"

namespace bzd::core {

class Executor
{
public:
	Executor(platform::Core& execution) : execution_{execution} {}

	constexpr void start() noexcept
	{
		const auto result = execution_.start();
		if (result.hasError())
		{
			log::print(CSTR("Error while initializing."));
		}
	}

	void dispatch()
	{
	}

	constexpr void stop()
	{
	}

private:
	platform::Core& execution_;
};

}
