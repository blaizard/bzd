#pragma once

#include "cc/bzd/container/optional.h"
#include "cc/bzd/container/result.h"
#include "cc/bzd/platform/core.h"
#include "cc/bzd/platform/core/stack.h"
#include "cc/bzd/utility/constexpr_for.h"

namespace bzd::platform::core {

template <class... Cores>
class Executor : public bzd::platform::adapter::Executor<Executor<Cores...>>
{
public:
	constexpr Executor(Cores&... cores) noexcept : cores_{&cores...}, executor_{}, start_{executor_, &bzd::Executor::run} {}

	/**
	 * Assign a workload to this executor.
	 */
	template <class V, class E>
	constexpr void enqueue(bzd::Async<V, E>& async) noexcept
	{
		async.enqueue(executor_);
	}

	/**
	 * Start the executor.
	 */
	void start() noexcept
	{
		constexprForContainerInc(cores_, [&](auto item) { item->start(start_); });
	}

	/**
	 * Stop the executor.
	 */
	void stop() noexcept
	{
		constexprForContainerDec(cores_, [](auto item) { item->stop(); });
	}

private:
	static constexpr bzd::SizeType nbCores_{sizeof...(Cores)};

	bzd::Tuple<Cores*...> cores_;
	bzd::Executor executor_;
	bzd::FunctionView<void(void)> start_;
};

} // namespace bzd::platform::core
