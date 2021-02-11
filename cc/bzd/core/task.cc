#include "bzd/core/task.h"

#include "bzd/core/scheduler.h"

void bzd::interface::Task::setActive() noexcept
{
	getScheduler().fromPendingToActive(*this);
}

void bzd::interface::Task::setPending() noexcept
{
	getScheduler().fromActiveToPending(*this);
}
