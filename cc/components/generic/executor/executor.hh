#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/interfaces/core.hh"
#include "cc/bzd/platform/interfaces/executor.hh"
#include "cc/bzd/utility/apply.hh"

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

	/// Assign a workload to this executor.
	constexpr void schedule(bzd::concepts::async auto& async, const bzd::async::Type type) noexcept { async.enqueue(executor_, type); }

	/// Start the executor.
	constexpr void start() noexcept
	{
		bzd::apply([&](auto&... cores) { (cores->start(start_), ...); }, cores_);
	}

	/// Stop the executor.
	constexpr void stop() noexcept
	{
		bzd::apply([](auto&... cores) { (cores->stop(), ...); }, cores_);
	}

private:
	static constexpr bzd::SizeType nbCores_{sizeof...(Cores)};

	bzd::Tuple<Cores*...> cores_;
	bzd::coroutine::impl::Executor executor_;
	bzd::FunctionRef<void(void)> start_;
};

} // namespace bzd::platform::generic
