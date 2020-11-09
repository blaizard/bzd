#pragma once

#include "bzd/container/function.h"
#include "bzd/utility/forward.h"
#include "bzd/core/assert.h"
#include "bzd/platform/stack.h"
#include "bzd/platform/types.h"

namespace bzd::interface {
class Task
{
public:
	Task(const FctPtrType fct) : fct_(fct) {}

	/**
	 * Bind a task to a stack
	 */
	void bind(platform::interface::Stack& stack)
	{
		bzd::assert::isTrue(stack_ == nullptr, "Task already bound to a stack.");
		stack_ = &stack;
		stack_->reset(fct_);
	}

	void start(platform::interface::Stack& stack)
	{
		bzd::assert::isTrue(stack_, "Task is not bounded to a stack.");
		stack.contextSwitch(*stack_);
	}

	/**
	 * Switch context and run this new task
	 */
	void yield(Task& nextTask)
	{
		bzd::assert::isTrue(stack_, "Task is not bounded to a stack.");
		stack_->contextSwitch(*nextTask.stack_);
	}

protected:
	const FctPtrType fct_;
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
		return []() { callable(); };
	}
};
} // namespace bzd
