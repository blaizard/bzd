#pragma once

#include <list>

#include "bzd/interface/task.h"
#include "bzd/utility/singleton.h"

namespace bzd
{
	class Scheduler : public SingletonThreadLocal<Scheduler>
	{
	public:
		/**
		 * Add a new task to the scheduler
		 */
		void addTask(bzd::interface::Task& task)
		{
			// Only add bounded tasks if (task.isBind())
			queue_.push_back(&task);
		}

		void start()
		{
			task_ = *(queue_.begin());
			queue_.pop_front();
			task_->start(&mainStack_);
		}

		void yield()
		{
			// Push back the current task
			queue_.push_back(task_);
			auto* previousTask = task_;
			task_ = *(queue_.begin());
			queue_.pop_front();
			previousTask->yield(*task_);
		}

	private:
		std::list<bzd::interface::Task*> queue_;
		bzd::interface::Task* task_;
		void* mainStack_ = nullptr;
	};

	void yield()
	{
		Scheduler::getInstance().yield();
	}
}
