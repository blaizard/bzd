#include "cc/bzd/platform/panic.h"

void bzd::platform::panic()
{
	throw 42;
}
