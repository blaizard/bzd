#include "bzd/core/task.h"

#include "bzd/core/scheduler.h"

void bzd::interface::Task::setActive() noexcept
{
	getScheduler().setActive(*this);
}
