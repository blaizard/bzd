#pragma once

#include "bzd/container/function.h"
#include "bzd/container/impl/non_owning_list.h"
#include "bzd/core/assert.h"
#include "bzd/platform/stack.h"
#include "bzd/platform/types.h"
#include "bzd/utility/forward.h"

namespace bzd {
class Scheduler;
void yield();
} // namespace bzd

namespace bzd::interface {
class Task : public bzd::NonOwningListElement</*MultiContainer*/ true>
{
public: // Types.
	using PtrType = Task*;
	enum class Status
	{
		IDLE = 0,
		RUNNING,
		TERMINATED
	};

public:
	Task() = default;

	bool hasStack() const noexcept { return stack_ != nullptr; }

	Status getStatus() const noexcept { return status_; }

protected:
	friend class bzd::Scheduler;

	platform::interface::Stack& getStack()
	{
		bzd::assert::isTrue(stack_, "Task is not bounded to a stack.");
		return *stack_;
	}

protected:
	Status status_{Status::IDLE};
	platform::interface::Stack* stack_{nullptr};
};

class TaskUser : public Task
{
public:
	TaskUser(const bzd::FctPtrType fct) : Task(), fct_(fct) {}

	/**
	 * Bind a task to a stack
	 */
	void bind(platform::interface::StackUser& stack)
	{
		bzd::assert::isTrue(stack_ == nullptr, "Task already bound to a stack.");
		stack.reset(fct_);
		stack_ = &stack;
	}

	void terminate()
	{
		status_ = Status::TERMINATED;
		bzd::yield();
	}

protected:
	const FctPtrType fct_;
};
} // namespace bzd::interface

namespace bzd {

/**
 * Creates a task.
 */
template <class Callable>
class Task : public interface::TaskUser
{
public:
	Task(Callable&& callable) : interface::TaskUser{callableWrapper(bzd::forward<Callable>(callable), this)} {}

private:
	static FctPtrType callableWrapper(Callable&& callable, interface::TaskUser* task)
	{
		// It is important to copy the task pointer and the callable
		static bzd::Function<void(void)> bind{[task, callable]() {
			callable();
			task->terminate();
		}};
		return []() { bind(); };
	}
};
} // namespace bzd
