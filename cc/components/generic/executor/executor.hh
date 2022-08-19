#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/interfaces/core.hh"
#include "cc/bzd/platform/interfaces/executor.hh"
#include "cc/bzd/utility/apply.hh"

namespace bzd::platform::generic {

template <Size n>
class Executor : public bzd::platform::Executor<Executor<n>>
{
public:
	using Self = Executor<n>;

public:
	template <class... Cores>
	constexpr Executor(Cores&... cores) noexcept : cores_{inPlace, &cores...}, executor_{}
	{
	}

	/// Assign a workload to this executor.
	constexpr void schedule(bzd::concepts::async auto& async, const bzd::async::Type type) noexcept
	{
		bzd::Optional<bzd::async::Executable::OnTerminateCallback> onTerminate{};
		if (type == bzd::async::Type::workload)
		{
			onTerminate = bzd::async::Executable::OnTerminateCallback::toMember<Self, &Self::onExecutableTerminate>(*this);
			++workloadCount_;
		}
		// Enqueue the executor and add a continuation.
		async.enqueue(executor_, type, onTerminate);
	}

	/// Start the executor.
	bzd::Result<void, bzd::Error> start() noexcept
	{
		const auto run = bzd::FunctionRef<void(bzd::platform::Core&)>::toMember<Self, &Self::run>(*this);
		for (auto core : cores_)
		{
			if (auto result = core->start(run); !result)
			{
				return bzd::move(result).propagate();
			}
		}
		return bzd::nullresult;
	}

	/// Stop the executor.
	bzd::Result<void, bzd::Error> stop() noexcept
	{
		for (auto core : cores_)
		{
			if (auto result = core->stop(); !result)
			{
				return bzd::move(result).propagate();
			}
		}
		executor_.shutdown();
		return bzd::nullresult;
	}

private:
	/// Call the idle function for each core.
	///
	/// \param index The core index the way it was registered within the executor.
	/// \param core The core instance.
	/// \return True if the core should continue running, false if it should be stopped.
	Bool idle(const Size index, bzd::platform::Core&) noexcept
	{
		// Only keep core 0 running.
		return (index == 0u);
	}

private:
	/// Run the executor.
	constexpr void run(bzd::platform::Core& core) noexcept
	{
		Bool runCore{true};
		do
		{
			executor_.run();
			if (workloadCount_.load() == 0u)
			{
				break;
			}
			const auto index = cores_.find(&core);
			bzd::assert::isTrue(index != bzd::npos, "Core must be registered.");
			runCore = idle(index, core);
		} while (runCore);
	}

	/// Callback triggered when an active async is terminated.
	constexpr bzd::Optional<bzd::async::Executable&> onExecutableTerminate() noexcept
	{
		if (--workloadCount_ == 0u)
		{
			bzd::log::info("All workloads are terminated, requesting shutdown.").sync();
			executor_.requestShutdown();
		}
		return bzd::nullopt;
	}

private:
	bzd::Array<bzd::platform::Core*, n> cores_;
	bzd::coroutine::impl::Executor executor_;
	bzd::Atomic<bzd::Size> workloadCount_{0};
};

template <class... Cores>
Executor(Cores&...) -> Executor<sizeof...(Cores)>;

} // namespace bzd::platform::generic
