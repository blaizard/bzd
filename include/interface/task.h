#pragma once

#include "include/interface/stack.h"
#include "include/types.h"

namespace async
{
	namespace impl
	{
		extern "C" void contextSwitch(void** stack1, void* stack2);
		extern "C" void* contextTask();
	}

	namespace interface
	{
		class Task
		{
		public:
			Task(ctx_ptr_type context, const fct_ptr_type fct)
				: context_(context), fct_(fct)
			{
			}

			/**
			 * Bind a task to a stack
			 */
			void bind(interface::Stack& stack)
			{
				stack_ = &stack;
				stack_->reset(fct_, context_);	
			}

			void start()
			{
				void* temp;
				impl::contextSwitch(&temp, stack_->stack_);
			}

			/**
			 * Switch context and run this new task
			 */
			void yield(Task& nextTask)
			{
				impl::contextSwitch(reinterpret_cast<void**>(&stack_->stack_), nextTask.stack_->stack_);
			}

		protected:
			ctx_ptr_type const context_;
			const fct_ptr_type fct_;
			interface::Stack* stack_;
		};
	}
}
