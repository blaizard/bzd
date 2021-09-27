#include "cc/bzd/platform/panic.hh"
#include "cc/bzd/platform/types.hh"

#include <exception>

namespace bzd::platform {
void panic()
{
	// Triggers a signal on POSIX which provoques a stack trace dump.
	std::terminate();
}
} // namespace bzd::platform
