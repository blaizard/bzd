#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/interfaces/core.hh"
#include "cc/bzd/platform/interfaces/executor.hh"
#include "cc/bzd/utility/apply.hh"

namespace bzd::platform::generic {

template <class... Cores>
class Executor : public bzd::platform::Executor<Executor<Cores...>>
{
public:
	using Self = Executor<Cores...>;

public:
	constexpr Executor(Cores&... cores) noexcept : cores_{inPlace, &cores...}, executor_{} {}

	/// Assign a workload to this executor.
	constexpr void schedule(bzd::concepts::async auto& async, const bzd::async::Type type) noexcept
	{
		bzd::Optional<bzd::async::Executable::OnTerminateCallback> onTerminate{};
		if (type == bzd::async::Type::active)
		{
			onTerminate = bzd::async::Executable::OnTerminateCallback::toMember<Self, &Self::onAsyncTerminate>(*this);
			++nbActive_;
		}
		// Enqueue the executor and add a continuation.
		async.enqueue(executor_, type, onTerminate);
	}

	/// Start the executor.
	constexpr void start() noexcept
	{
		const auto run = bzd::FunctionRef<void(void)>::toMember<Self, &Self::run>(*this);
		bzd::apply([run](auto&... cores) { (cores->start(run), ...); }, cores_);
	}

	/// Stop the executor.
	constexpr void stop() noexcept
	{
		bzd::apply([](auto&... cores) { (cores->stop(), ...); }, cores_);
	}

private:
	constexpr void run() noexcept { executor_.run(); }

	constexpr bzd::Optional<bzd::async::Executable&> onAsyncTerminate() noexcept
	{
		if (--nbActive_ == 0u)
		{
			bzd::log::info("All apps are terminated, requesting shutdown.").sync();
			executor_.requestShutdown();
		}
		return bzd::nullopt;
	}

private:
	bzd::Tuple<Cores*...> cores_;
	bzd::coroutine::impl::Executor executor_;
	bzd::Atomic<bzd::SizeType> nbActive_{0};
};

} // namespace bzd::platform::generic
