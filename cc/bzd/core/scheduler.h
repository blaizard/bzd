#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/queue.h"
#include "bzd/core/assert.h"
#include "bzd/core/promise.h"
#include "bzd/core/task.h"
#include "bzd/utility/move.h"
#include "bzd/utility/singleton.h"
#include "bzd/utility/swap.h"

namespace bzd {
bzd::Scheduler& getScheduler();
void yield();
} // namespace bzd

namespace bzd {

class MainTask : public bzd::interface::TaskUser
{
public:
	MainTask() : interface::TaskUser{[]() {}} { stack_ = &mainStack_; }

private:
	bzd::platform::interface::Stack mainStack_{};
};

class Scheduler : public SingletonThreadLocal<Scheduler>
{
private:
	using TaskPtr = bzd::interface::Task*;

public:
	/**
	 * Add a new task to the scheduler
	 */
	void addTask(TaskPtr task)
	{
		// Only add bounded tasks if (task.isBind())
		bzd::assert::isTrue(task->hasStack(), "Task can only be added if bounded to a stack.");
		queue_.push(task);
	}

	void start()
	{
		task_ = &mainTask_;
		auto maybeTask = getNextTask();
		if (maybeTask)
		{
			resumeTask(*maybeTask);
		}
	}

	template <class V, class E>
	typename bzd::Promise<V, E>::ResultType&& await(bzd::Promise<V, E>& promise)
	{
		while (!promise.poll())
		{
			bzd::yield();
		}
		return bzd::move(promise.getResult());
	}

	void yield()
	{
		if (task_->getStatus() != bzd::interface::Task::Status::TERMINATED)
		{
			queue_.push(bzd::move(task_));
		}

		// Push back the current task
		auto maybeTask = getNextTask();

		if (maybeTask)
		{
			resumeTask(*maybeTask);
		}
		else
		{
			resumeTask(&mainTask_);
		}
	}

private:
	/**
	 * Get the next task to be executed from teh main task queue.
	 */
	bzd::Optional<TaskPtr> getNextTask() noexcept
	{
		if (queue_.empty())
		{
			return bzd::nullopt;
		}
		auto task = queue_.front();
		queue_.pop();
		return task;
	}

	/**
	 * Resume a task and set it as the current task.
	 */
	void resumeTask(TaskPtr task)
	{
		auto previousTask = task_;
		task_ = task;
		previousTask->getStack().shift(task_->getStack());
	}

private:
	bzd::Queue<TaskPtr, 10> queue_;
	TaskPtr task_;
	MainTask mainTask_{};
};

} // namespace bzd

namespace bzd::impl {
struct AwaitType
{
	explicit constexpr AwaitType() noexcept {}
};
constexpr AwaitType await_{};

template <class T>
auto&& operator>(const AwaitType&, T&& promise)
{
	return bzd::move(getScheduler().await(promise));
}
} // namespace bzd::impl

// Syntax suggar for await operator
#define await bzd::impl::await_ >
