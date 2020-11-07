#pragma once

#include "bzd/container/function.h"
#include "bzd/utility/forward.h"
#include "example/task_interface.h"

namespace bzd {

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
