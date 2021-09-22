#include "cc/bzd/platform/panic.hh"

void bzd::platform::panic()
{
	throw 42;
}
