#include "include/format.h"

#include <iostream>

int main()
{
	bzd::String<32> str;
	bzd::format::toString(str, "Hello {i}", 42);

	std::cout << str.data() << std::endl;

	return 0;
}
