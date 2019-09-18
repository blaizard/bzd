#pragma once

#include "include/interface/task.h"
#include <iostream>

namespace async
{
/*
	#define WRAPPER(FUNC) async::wrapper<decltype(&FUNC), &FUNC>

	template<class Fn, Fn fn, class... Args>
	typename std::result_of<Fn(Args...)>::type wrapper(Args... args)
	{
		std::cout << "pre-wrap" << std::endl;
		const auto result = fn(std::forward<Args>(args)...);
		std::cout << "post-wrap, return " << result << std::endl;
		return result;
	}
*/
	template <class ObjPtr, class FctPtr>
	class Task : public interface::Task
	{
	private:
		struct Context
		{
			ObjPtr obj_;
			const FctPtr fct_;
			int i_;
		};

	public:
		Task(interface::Stack& stack, ObjPtr obj, const FctPtr fct)
			: interface::Task(stack, static_cast<this_ptr_type>(&context_), reinterpret_cast<fct_ptr_type>(wrapper))
			, context_{obj, fct, 42}
		{
		}

		static void wrapper()
		{
			Context* context = reinterpret_cast<Context*>(async::impl::contextGet());
			std::cout << "Enter wrapper" << std::endl;
			std::cout << context->i_ << std::endl;
			((context->obj_)->*(context->fct_))(); //std::forward<Args>(args)...);
		}

	private:
		Context context_;
	};
}
