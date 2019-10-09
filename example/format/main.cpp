#include "include/format.h"

#include <iostream>

int main()
{
	static constexpr const int a = 42;
	bzd::String<32> str;
	bzd::format(str, "Hello {i}", a);

	std::cout << str.data() << std::endl;

	return 0;
}
