#include "cc/bzd.h"

int main()
{
	bzd::String<32> str;
	bzd::interface::StringChannel sstream(str);

	bzd::format::toString(str, 12);
	bzd::platform::getOut().write(str.asBytes());

	/*	str.clear();
		bzd::format::toStreamBin(sstream, 12);
		bzd::getOut().write(str);

		str.clear();
		bzd::format::toStreamHex(sstream, 748787);
		bzd::getOut().write(str);
	*/
	return 0;
}
