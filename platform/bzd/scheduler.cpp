#include "bzd/scheduler.h"

namespace bzd {

void yield()
{
	Scheduler::getInstance().yield();
}

} // namespace bzd
