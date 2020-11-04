#include "bzd/platform/system.h"

void bzd::platform::panic()
{
	throw 42;
}
