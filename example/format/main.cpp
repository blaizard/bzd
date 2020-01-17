#include "bzd.h"

int main()
{
	bzd::format::toString(bzd::getOut(), CSTR("The answer is {}.\n"), 42);

	return 0;
}
