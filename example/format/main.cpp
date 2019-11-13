#include <iostream>
#include "bzd/format/format.h"
#include "bzd/system.h"

int main()
{
	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("Hello {}"), 12);

	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("Hello It'se I am here! {}"), 'a');

	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("Hello It's me I am h! {} {}"), 12, 3);

	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("HelIt's me I am here! {} {} {}"), 12, 2, false);

	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("Hello It's  I am here! {} {} {} {}"), 12, 5.4454, -323, 32323);

	//formatMe(CONSTEXPR_STRING_VIEW("Hello {:s}"), 45);

/*
	static constexpr const int a = 42;
	bzd::String<32> str;
	bzd::format(str, "Hello {:i}", a);
	bzd::format(str, "Hello {:i} {}", 978877897897897llu, 4);
	bzd::format(str, "Hello {:i}", 45.5);
	bzd::format(str, "Hello {} {} {}", a, 45, 5);
	bzd::format(str, "{} {} {} {}", a, 45, 5, -4);

	std::cout << str.data() << std::endl;
*/
	return 0;
}
