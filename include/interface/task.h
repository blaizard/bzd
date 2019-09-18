#pragma once

#include "include/interface/stack.h"
#include "include/types.h"

namespace async
{
	namespace impl
	{
		extern "C" void contextSwitch(void** stack1, void* stack2);
		extern "C" void* contextGet();
	}

	namespace interface
	{
		class Task
		{
		public:
			Task(interface::Stack& stack, this_ptr_type obj, const fct_ptr_type fct)
				: stack_(stack)
			{
				stack_.reset(fct, obj);
			}

			Task(interface::Stack& stack, const fct_ptr_type fct)
				: stack_(stack)
			{
				stack_.reset(fct, nullptr);
			}

			void start()
			{
				void* temp;
				impl::contextSwitch(&temp, stack_.stack_);
			}

			/**
			 * Switch context and run this new task
			 */
			void yield(Task& nextTask)
			{
				impl::contextSwitch(reinterpret_cast<void**>(&stack_.stack_), nextTask.stack_.stack_);
			}

		protected:
			interface::Stack& stack_;
		};
	}
}
