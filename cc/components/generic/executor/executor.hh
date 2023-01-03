#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/interfaces/core.hh"
#include "cc/bzd/platform/interfaces/executor.hh"
#include "cc/bzd/utility/apply.hh"

namespace bzd::platform::generic {

template <class Config>
class Executor : public bzd::platform::Executor<Executor<Config>>
{
public:
	using Self = Executor<Config>;

public:
	constexpr Executor(Config& config) noexcept : config_{config}, executor_{}
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
		for (auto& core : config_.cores)
		{
			if (auto result = core.start(run); !result)
			{
				return bzd::move(result).propagate();
			}
		}
		return bzd::nullresult;
	}

	/// Stop the executor.
	bzd::Result<void, bzd::Error> stop() noexcept
	{
		for (auto& core : config_.cores)
		{
			if (auto result = core.stop(); !result)
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
			// Get the index of the core in the array.
			const auto it = bzd::algorithm::findIf(config_.cores, [&core](auto& value) { return &core == &value; });
			bzd::assert::isTrue(it != config_.cores.end(), "Core must be registered.");
			const auto index = bzd::distance(config_.cores.begin(), it);
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
	Config& config_;
	bzd::coroutine::impl::Executor executor_;
	bzd::Atomic<bzd::Size> workloadCount_{0};
};

} // namespace bzd::platform::generic
