#pragma once

#include "cc/bzd/platform/core.h"
#include "cc/bzd/core/log.h"
#include "cc/bzd/core/async.h"

namespace bzd::core {

class Executor
{
public:
	Executor(platform::Core& execution) : execution_{execution} {}

	constexpr void start() noexcept
	{
		(void) execution_;
		/*const auto result = execution_.start();
		if (result.hasError())
		{
			log::print(CSTR("Error while initializing."));
		}*/
	}

	template <class... Asyncs>
	constexpr void run(Asyncs&&... asyncs)
	{
		// Assign asyncs to their executors

		auto promiseAll = bzd::async::all(bzd::forward<Asyncs>(asyncs)...);
		promiseAll.attach();
		bzd::Scheduler::getInstance().run();
	}

	constexpr void stop()
	{
	}

private:
	//bzd::Vector<platform::Core&>
	platform::Core& execution_;
};

}
