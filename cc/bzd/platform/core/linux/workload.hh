#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/delay.hh"

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
