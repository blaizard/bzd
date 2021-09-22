#include "cc/bzd/platform/panic.hh"

#include <exception>

void bzd::platform::panic()
{
	// Propagate a signal on POSIX which provoques a stack trace dump.
	std::terminate();
}
