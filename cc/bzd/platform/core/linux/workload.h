#pragma once

#include "cc/bzd/core/async.h"
#include "cc/bzd/core/delay.h"

bzd::Async<int> worload(int var)
{
	while (var > 0)
	{
		--var;
		std::cout << "Number: " << var << std::endl;
		co_await bzd::delay(500_ms);
	};
	co_return var;
}
