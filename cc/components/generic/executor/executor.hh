#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/interfaces/core.hh"
#include "cc/bzd/platform/interfaces/executor.hh"
#include "cc/bzd/utility/constexpr_for.hh"

namespace bzd::platform::generic {

template <class... Cores>
class Executor : public bzd::platform::adapter::Executor<Executor<Cores...>>
{
public:
	constexpr Executor(Cores&... cores) noexcept :
		cores_{inPlace, &cores...}, executor_{},
		start_{bzd::FunctionRef<void(void)>::toMember<decltype(executor_), &bzd::coroutine::impl::Executor::run>(executor_)}
	{
	}

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
	bzd::coroutine::impl::Executor executor_;
	bzd::FunctionRef<void(void)> start_;
};

} // namespace bzd::platform::generic
