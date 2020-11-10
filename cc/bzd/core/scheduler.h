#pragma once

#include "bzd/container/queue.h"
#include "bzd/core/task.h"
#include "bzd/utility/move.h"
#include "bzd/utility/singleton.h"

namespace bzd {
class Scheduler : public SingletonThreadLocal<Scheduler>
{
public:
	/**
	 * Add a new task to the scheduler
	 */
	void addTask(bzd::interface::Task& task)
	{
		// Only add bounded tasks if (task.isBind())
		queue_.push(&task);
	}

	void start()
	{
		task_ = queue_.front();
		queue_.pop();
		task_->resume(mainStack_);
	}

	void yield()
	{
		if (task_->getStatus() != bzd::interface::Task::Status::TERMINATED)
		{
		}

		// Push back the current task
		auto previousTask = queue_.push(bzd::move(task_));
		task_ = queue_.front();
		queue_.pop();
		task_->resume(previousTask->getStack());
	}

private:
	bzd::Queue<bzd::interface::Task*, 10> queue_;
	bzd::interface::Task* task_;
	bzd::platform::interface::Stack mainStack_{};
};

void yield();

template <class T>
void await(T&&)
{
	yield();
}

bzd::Scheduler& getScheduler();

} // namespace bzd

#define bzd_yield bzd::yield()
