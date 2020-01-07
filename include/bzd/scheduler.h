#pragma once

#include "bzd/container/queue.h"
#include "bzd/interface/task.h"
#include "bzd/utility/move.h"
#include "bzd/utility/singleton.h"

namespace bzd {
class Scheduler : public SingletonThreadLocal<Scheduler>
{
public:
	/**
	 * Add a new task to the scheduler
	 */
	void addTask(bzd::interface::Task &task)
	{
		// Only add bounded tasks if (task.isBind())
		queue_.push(&task);
	}

	void start()
	{
		task_ = queue_.front();
		queue_.pop();
		task_->start(&mainStack_);
	}

	void yield()
	{
		// Push back the current task
		auto *previousTask = task_;
		queue_.push(bzd::move(task_));
		task_ = queue_.front();
		queue_.pop();
		previousTask->yield(*task_);
	}

private:
	bzd::Queue<bzd::interface::Task *, 10> queue_;
	bzd::interface::Task *task_;
	void *mainStack_ = nullptr;
};

void yield()
{
	Scheduler::getInstance().yield();
}
} // namespace bzd
