#pragma once

#include "bzd/interface/stack.h"
#include "bzd/types.h"

namespace bzd {
namespace impl {
extern "C" void contextSwitch(void** stack1, void* stack2);
extern "C" void* contextTask();
} // namespace impl

namespace interface {
class Task
{
public:
	Task(PtrType context, const FctPtrType fct) : context_(context), fct_(fct) {}

	/**
	 * Bind a task to a stack
	 */
	void bind(interface::Stack& stack)
	{
		stack_ = &stack;
		stack_->reset(fct_, context_);
	}

	void start(void** pointer) { impl::contextSwitch(pointer, stack_->stack_); }

	/**
	 * Switch context and run this new task
	 */
	void yield(Task& nextTask)
	{
		impl::contextSwitch(reinterpret_cast<void**>(&stack_->stack_), reinterpret_cast<void*>(nextTask.stack_->stack_));
	}

protected:
	PtrType const context_;
	const FctPtrType fct_;
	interface::Stack* stack_;
};
} // namespace interface
} // namespace bzd
