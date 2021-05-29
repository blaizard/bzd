#include "cc/bzd/platform/panic.h"

#include <stdlib.h>

void bzd::platform::panic()
{
	exit(42);
}
