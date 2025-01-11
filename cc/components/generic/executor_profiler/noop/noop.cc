#include "cc/components/generic/executor_profiler/noop/noop.hh"

namespace bzd::components::generic {

ExecutorProfilerNoop& getExecutorProfilerNoop() noexcept
{
	static ExecutorProfilerNoop profiler{};
	return profiler;
}

} // namespace bzd::components::generic
