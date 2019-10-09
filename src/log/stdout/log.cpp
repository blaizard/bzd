#include "include/system.h"

#include <iostream>

void bzd::print(const bzd::StringView& message)
{
	std::cout << message.data() << std::endl;
}
