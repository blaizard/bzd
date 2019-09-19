#pragma once

#include "include/interface/task.h"
#include <iostream>

namespace async
{
	template <class T>
	struct resultOf;
	
	template <class T, class... Args>
	struct resultOf<T(&)(Args...)>
	{
		typedef T type;
	};
	

	template <class T, class... Args>
	class Task : public interface::Task
	{
	private:
		struct Context
		{
			const T& callable_;
		};

	public:
		Task(const T& callable)
			: interface::Task(static_cast<ctx_ptr_type>(&context_), reinterpret_cast<fct_ptr_type>(wrapper<typename resultOf<decltype(callable)>::type>))
			, context_{callable}
		{
		}

		template <class R>
		static void wrapper()
		{
			Context* context = reinterpret_cast<Context*>(async::impl::contextTask());
			context->callable_();
		}

	private:
		Context context_;
	};
}
