#include "bzd.h"

int main()
{
	bzd::String<32> str;
	bzd::interface::StringChannel sstream(str);

	bzd::format::toString(str, 12);
	bzd::getOut().write(str);

	/*	str.clear();
		bzd::format::toStringBin(sstream, 12);
		bzd::getOut().write(str);

		str.clear();
		bzd::format::toStringHex(sstream, 748787);
		bzd::getOut().write(str);
	*/
	return 0;
}
