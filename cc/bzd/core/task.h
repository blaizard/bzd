#pragma once

#include "bzd/container/function.h"
#include "bzd/core/assert.h"
#include "bzd/platform/stack.h"
#include "bzd/platform/types.h"
#include "bzd/utility/forward.h"

#include <iostream>
#include <stdlib.h>

namespace bzd {
void yield();
}

namespace bzd::interface {
class Task
{
protected:
	using FctPtrType = bzd::platform::interface::Stack::FctPtrType;
	using TaskPtrType = bzd::platform::interface::Stack::TaskPtrType;

public:
	enum class Status
	{
		IDLE = 0,
		RUNNING,
		TERMINATED
	};

public:
	Task(const bzd::platform::interface::Stack::FctPtrType fct) : fct_(fct) {}

	/**
	 * Bind a task to a stack
	 */
	void bind(platform::interface::Stack& stack)
	{
		bzd::assert::isTrue(stack_ == nullptr, "Task already bound to a stack.");
		stack_ = &stack;
		stack_->reset(fct_, this);
	}

	/**
	 * Resume this task by switching to this task's stack.
	 *
	 * \param stack The current stack, this is where the stack pointer will be stored.
	 */
	void resume(platform::interface::Stack& stack) { stack.contextSwitch(getStack()); }

	void terminate()
	{
		status_ = Status::TERMINATED;
		bzd::yield();
	}

	Status getStatus() const noexcept { return status_; }

	platform::interface::Stack& getStack()
	{
		bzd::assert::isTrue(stack_, "Task is not bounded to a stack.");
		return *stack_;
	}

protected:
	const FctPtrType fct_;
	Status status_{Status::IDLE};
	platform::interface::Stack* stack_{nullptr};
};
} // namespace bzd::interface

namespace bzd {

/**
 * Creates a task.
 */
template <class Callable>
class Task : public interface::Task
{
public:
	Task(Callable&& callable) : interface::Task{callableWrapper(bzd::forward<Callable>(callable))} {}

private:
	static FctPtrType callableWrapper(Callable&& instance)
	{
		static bzd::Function<void(void)> callable{bzd::forward<Callable>(instance)};
		return [](const TaskPtrType task) {
			callable();
			task->terminate();
		};
	}
};
} // namespace bzd
