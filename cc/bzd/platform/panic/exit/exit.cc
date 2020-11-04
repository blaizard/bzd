#include "bzd/platform/system.h"

#include <stdlib.h>

void bzd::platform::panic()
{
	exit(42);
}
