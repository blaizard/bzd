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
	 *
	 * \note The task cannot be associated with a scheduler already,
	 * must have a stack and must be in idle mode.
	 */
	void addTask(Task& task) noexcept;

	void start()
	{
		task_ = &mainTask_;
		auto maybeTask = getNextTask();
		if (maybeTask)
		{
			resumeTask(maybeTask.valueMutable());
		}
	}

	template <class V, class E>
	constexpr typename bzd::Promise<V, E>::ResultType&& await(bzd::Promise<V, E>& promise)
	{
		// Associate the promise with the task
		task_->registerPromise(promise);

		// Use a tri-state return code from poll() instead to remove isEvent
		while (!promise.poll())
		{
			bzd::yield();
		}

		return bzd::move(promise.getResult());
	}

	void yield()
	{
		do
		{
			// Push back the current task
			if (task_->getStatus() != bzd::interface::Task::Status::TERMINATED &&
				task_->getStatus() != bzd::interface::Task::Status::PENDING)
			{
				tasks_.pushFront(*task_);
			}

			// Switch to the next task
			auto maybeTask = getNextTask();
			if (maybeTask)
			{
				return resumeTask(maybeTask.valueMutable());
			}

			// If no next task, loop until there are still pending tasks
		} while (!pendingTasks_.empty());

		// Else return to the main stack
		resumeTask(mainTask_);
	}

	/**
	 * Set the current task to pending state.
	 */
	void fromActiveToPending(bzd::interface::Task& task) noexcept
	{
		bzd::assert::isTrue(task.getStatus() == bzd::interface::Task::Status::ACTIVE, "Task is not active.");

		const auto resultPop = tasks_.pop(task);
		bzd::assert::isTrue(resultPop.hasValue() || resultPop.error() == ListErrorType::elementAlreadyRemoved,
							"Cannot remove task from task list.");

		const auto resultPush = pendingTasks_.pushFront(task);
		bzd::assert::isTrue(resultPush.hasValue(), "Cannot push task to pending list.");

		task.status_ = bzd::interface::Task::Status::PENDING;
	}

	/**
	 * Activate a task currently pending.
	 */
	void fromPendingToActive(bzd::interface::Task& task) noexcept
	{
		bzd::assert::isTrue(task.getStatus() == bzd::interface::Task::Status::PENDING, "Task is not pending.");
		task.status_ = bzd::interface::Task::Status::ACTIVE;
		pendingTasks_.pop(task);
		tasks_.pushFront(task);
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
		tasks_.pop(result.valueMutable());
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
	bzd::NonOwningList<Task> tasks_{};
	bzd::NonOwningList<Task> pendingTasks_{};
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
