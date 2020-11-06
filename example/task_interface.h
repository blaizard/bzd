#pragma once

#include "bzd/core/assert.h"
#include "bzd/platform/stack.h"
#include "bzd/platform/system.h"
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
		bzd::assert::isTrue(stack_ == nullptr, "Stack already bound to a stack.");
		stack_ = &stack;
		stack_->reset(fct_);
	}

	void start(platform::interface::Stack& stack)
	{
		bzd::assert::isTrue(stack_, "Stack is not bounded to a stack.");
		stack.contextSwitch(*stack_);
	}

	/**
	 * Switch context and run this new task
	 */
	void yield(Task& nextTask)
	{
		bzd::assert::isTrue(stack_, "Stack is not bounded to a stack.");
		stack_->contextSwitch(*nextTask.stack_);
	}

protected:
	const FctPtrType fct_;
	platform::interface::Stack* stack_{nullptr};
};
} // namespace bzd::interface
