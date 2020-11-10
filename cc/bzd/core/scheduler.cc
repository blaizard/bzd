#include "bzd/core/scheduler.h"

namespace bzd {

void yield()
{
	Scheduler::getInstance().yield();
}

Scheduler& getScheduler()
{
	return Scheduler::getInstance();
}

} // namespace bzd
