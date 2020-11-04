#include "example/scheduler.h"

namespace bzd {

void yield()
{
	Scheduler::getInstance().yield();
}

} // namespace bzd
