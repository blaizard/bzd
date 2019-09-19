#pragma once

#include <list>

#include "include/interface/task.h"

namespace async
{
	class Scheduler
	{
	public:
		Scheduler() = default;

		void addTask(async::interface::Task& task)
		{
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
			// Push bash the task
			queue_.push_back(task_);
			auto* previousTask = task_;
			task_ = *(queue_.begin());
			queue_.pop_front();
			previousTask->yield(*task_);
		}

	private:
		std::list<async::interface::Task*> queue_;
		async::interface::Task* task_;
		void* mainStack_ = nullptr;
	};
}
