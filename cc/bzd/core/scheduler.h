#pragma once

#include "bzd/container/impl/non_owning_list.h"
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

namespace bzd::impl {

class MainTask : public bzd::interface::TaskUser
{
public:
	MainTask() : interface::TaskUser{[]() {}} { stack_ = &mainStack_; }

private:
	bzd::platform::interface::Stack mainStack_{};
};
} // namespace bzd::impl

namespace bzd {
class Scheduler : public SingletonThreadLocal<Scheduler>
{
private:
	using Task = bzd::interface::Task;

public:
	/**
	 * Add a new task to the scheduler.
	 */
	void addTask(Task& task)
	{
		// Only add bounded tasks if (task.isBind())
		bzd::assert::isTrue(task.hasStack(), "Task can only be added if bounded to a stack.");
		tasks_.pushFront(task);
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
	constexpr typename bzd::impl::Promise<V, E>::ResultType&& await(bzd::impl::Promise<V, E>& promise)
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
			tasks_.pushFront(*task_);
		}

		// Push back the current task
		auto maybeTask = getNextTask();
		if (maybeTask)
		{
			resumeTask(*maybeTask);
		}
		else
		{
			resumeTask(mainTask_);
		}
	}

private:
	/**
	 * Get the next task to be executed from teh main task queue.
	 */
	bzd::Optional<Task&> getNextTask() noexcept
	{
		auto result = tasks_.back();
		if (!result)
		{
			return bzd::nullopt;
		}
		tasks_.pop(*result);
		return result;
	}

	/**
	 * Resume a task and set it as the current task.
	 */
	void resumeTask(Task& task)
	{
		auto previousTask = task_;
		task_ = &task;
		previousTask->getStack().shift(task_->getStack());
	}

private:
	// Main task list, which contains only active tasks.
	bzd::impl::NonOwningList<Task> tasks_{};
	// The current task
	Task* task_{nullptr};
	impl::MainTask mainTask_{};
};

} // namespace bzd

namespace bzd::impl {
struct AwaitType
{
	explicit constexpr AwaitType() noexcept {}
};
constexpr AwaitType await_{};

template <class T>
constexpr auto&& operator>(const AwaitType&, T&& promise)
{
	return bzd::move(getScheduler().await(promise));
}
} // namespace bzd::impl

// Syntax suggar for await operator
#define await bzd::impl::await_ >
