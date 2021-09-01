#pragma once

#include "cc/bzd/core/async.h"

bzd::Async<int> worload(int var)
{
	std::cout << "Number: " << var << std::endl;
	co_return var;
}
