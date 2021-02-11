#include "bzd/core/scheduler.h"

namespace bzd {

void Scheduler::addTask(Task& task) noexcept
{
	// Only add bounded tasks if (task.isBind())
	bzd::assert::isTrue(task.hasStack(), "Task can only be added if bounded to a stack.");
	bzd::assert::isTrue(task.getStatus() == bzd::interface::Task::Status::IDLE, "Task is not idle.");

	task.status_ = bzd::interface::Task::Status::ACTIVE;
	const auto result = tasks_.pushFront(task);
	bzd::assert::isTrue(result, "Cannot add task to the list.");
}

void yield()
{
	Scheduler::getInstance().yield();
}

Scheduler& getScheduler()
{
	return Scheduler::getInstance();
}

} // namespace bzd
