#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/utility/apply.hh"
#include "interfaces/core.hh"
#include "interfaces/executor.hh"

namespace bzd::components::generic {

template <class Context>
class Executor : public bzd::Executor<Executor<Context>>
{
public:
	using Self = Executor<Context>;

public:
	constexpr Executor(Context& context) noexcept : context_{context}, executor_{}, logger_{context.config.out} {}

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
		const auto run = bzd::FunctionRef<void(bzd::Core&)>::toMember<Self, &Self::run>(*this);
		for (auto& core : context_.config.cores)
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
		for (auto& core : context_.config.cores)
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
	Bool idle(const Size index, bzd::Core&) noexcept
	{
		// Only keep core 0 running.
		return (index == 0u);
	}

private:
	/// Run the executor.
	constexpr void run(bzd::Core& core) noexcept
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
			const auto it = bzd::algorithm::findIf(context_.config.cores, [&core](auto& value) { return &core == &value; });
			bzd::assert::isTrue(it != context_.config.cores.end(), "Core must be registered.");
			const auto index = bzd::distance(context_.config.cores.begin(), it);
			runCore = idle(index, core);
		} while (runCore);
	}

	/// Callback triggered when an active async is terminated.
	constexpr bzd::Optional<bzd::async::Executable&> onExecutableTerminate() noexcept
	{
		if (--workloadCount_ == 0u)
		{
			logger_.info("All workloads are terminated, requesting shutdown.").sync();
			executor_.requestShutdown();
		}
		return bzd::nullopt;
	}

private:
	Context& context_;
	bzd::async::Executor executor_;
	bzd::Logger logger_;
	bzd::Atomic<bzd::Size> workloadCount_{0};
};

} // namespace bzd::components::generic
