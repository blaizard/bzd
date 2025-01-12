#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/container/threadsafe/non_owning_ring_spin.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/async/executable.hh"
#include "cc/bzd/core/async/executor_profiler.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/ranges/associate_scope.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"
#include "cc/components/generic/executor_profiler/noop/core_profiler.hh"

namespace bzd::async::impl {

template <class Executable>
class Executor;

/// Executor context.
template <class Executable>
class ExecutorContext : public bzd::threadsafe::NonOwningForwardListElement</*multi container*/ false>
{
public: // Traits.
	using Executor = bzd::async::impl::Executor<Executable>;
	using IdType = bzd::async::profiler::UIdType;
	using TickType = UInt32;
	using OnTerminateCallback = bzd::FunctionRef<bzd::Optional<Executable&>(void)>;
	using Continuation = bzd::Variant<bzd::Monostate, Executable*, OnTerminateCallback>;

public:
	constexpr ExecutorContext(const UInt16 coreUId) noexcept : coreUId_{coreUId}, contextUId_{makeUId()} {}

	/// Get the unique identifier of this context across this executor.
	[[nodiscard]] constexpr IdType getUId() const noexcept { return contextUId_; }
	/// Get the unique identifier of this context across this executor.
	[[nodiscard]] constexpr IdType getCoreUId() const noexcept { return coreUId_; }
	/// Get the current tick for this executor.
	[[nodiscard]] constexpr TickType getTick() const noexcept { return tick_; }
	/// Provide an executable to be enqued sequentially, after the execution of the current executable.
	constexpr void setContinuation(Continuation&& continuation) noexcept
	{
		bzd::assert::isTrue(continuation_.template is<bzd::Monostate>());
		continuation_ = bzd::move(continuation);
	}

private:
	friend class bzd::async::impl::Executor<Executable>;

	constexpr void updateTick() noexcept { ++tick_; }

	[[nodiscard]] constexpr bzd::Optional<Executable&> popContinuation() noexcept
	{
		bzd::Optional<Executable&> maybeExecutable{};
		continuation_.match([](bzd::Monostate) noexcept {},
							[&](Executable* executable) noexcept {
								if (executable)
								{
									maybeExecutable.emplace(*executable);
								}
							},
							[&](OnTerminateCallback callback) noexcept { maybeExecutable = callback(); });
		continuation_.template emplace<bzd::Monostate>();
		return maybeExecutable;
	}

private:
	[[nodiscard]] static IdType makeUId() noexcept
	{
		static Atomic<IdType> id{0};
		return ++id;
	}

private:
	const UInt16 coreUId_;
	const IdType contextUId_;
	TickType tick_{0};
	Continuation continuation_{};
};

/// The executor concept is a workload scheduler that owns several executables
/// and executes them.
/// An executor is thread-safe and can be shared between multiple threads or cores.
template <class Executable>
class Executor
{
public: // Traits.
	using Self = Executor<Executable>;
	using TickType = typename ExecutorContext<Executable>::TickType;

	enum class Status
	{
		idle,
		running,
		shutdownRequested,
		abortRequested
	};

public:
	constexpr Executor() noexcept = default;

	// Copy/move constructor/assignment
	constexpr Executor(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Executor(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

	constexpr ~Executor() noexcept { shutdown(); }

public: // Statistics.
	[[nodiscard]] constexpr Size getQueueCount() const noexcept { return queueCount_.load(); }
	[[nodiscard]] constexpr Size getRunningCount() const noexcept { return context_.size(); }
	[[nodiscard]] constexpr Size getMaxRunningCount() const noexcept { return maxRunningCount_.load(); }
	[[nodiscard]] constexpr Int32 getWorkloadCount() const noexcept { return workloadCount_.load(); }

public:
	/// Run all the workload currently in the queue.
	///
	/// Drain all workloads and exit only when the queue is empty.
	///
	/// \param coreUId The core unique identifier on which this context is running.
	/// \param profiler The profiler to be used to profile the activity on this context.
	template <class Profiler = bzd::components::generic::CoreProfilerNoop>
	void run(const UInt16 coreUId, Profiler& profiler = bzd::components::generic::getCoreProfilerNoop()) noexcept
	{
		// Storage for context related to the this running instance.
		ExecutorContext<Executable> context{coreUId};
		auto scope = registerContext(context);
		auto scopeProfiler = registerProfiler(profiler, context);

		// Set the status.
		{
			auto expected{Status::idle};
			status_.compareExchange(expected, Status::running);
		}

		// Run at least the number of time there are elements in the queue, this is to ensure that
		// all the services are running at least once.
		const auto minIterationCount = getQueueCount();

		// Loop until there are still workload executables to process or an abort request is present.
		while ((getWorkloadCount() > 0 || context.getTick() <= minIterationCount) && status_.load() != Status::abortRequested)
		{
			// Drain remaining handles
			auto maybeExecutable = pop(context);
			if (maybeExecutable.hasValue())
			{
				profiler.event(profiler::ExecutableScheduled{});
				do
				{
					auto& executable = maybeExecutable.valueMutable();
					const auto isCanceled = executable.isCanceled();
					if (isCanceled)
					{
						profiler.event(profiler::ExecutableCanceled{});
						executable.cancel(context);
					}
					else
					{
						executable.resume(context);
					}

					// Execute the continuation if any, this instead of enqueuing it,
					// as it will speed up execution by avoiding unnecessary atomic operations,
					// keeping continuation running on the same core to please caching
					// and reduce the number of spin locks required to update the queue.
					maybeExecutable = context.popContinuation();
				} while (maybeExecutable.hasValue());
				profiler.event(profiler::ExecutableUnscheduled{});
			}
			context.updateTick();
		}
	}

	void requestShutdown() noexcept
	{
		auto expected{Status::running};
		status_.compareExchange(expected, Status::shutdownRequested);
	}

	void requestAbort() noexcept
	{
		auto expected{Status::running};
		do
		{
			if (status_.compareExchange(expected, Status::abortRequested))
			{
				break;
			}
		} while (expected != Status::idle);
	}

	constexpr Bool isRunning() const noexcept { return status_.load() == Status::running; }

	/// Schedule a new executable on this executor.
	constexpr void schedule(Executable& executable, const ExecutableMetadata::Type type) noexcept
	{
		schedule(executable, ExecutableMetadata{type});
	}

	constexpr void schedule(Executable& executable, const ExecutableMetadata& metadata) noexcept
	{
		executable.setMetadata(metadata);
		executable.setExecutor(*this);
		push(executable);
	}

	/// Shutdown the executor. This removes all the pending tasks and no other
	/// scheduling can proceed.
	void shutdown() noexcept
	{
		// Clear the complete queue.
		queue_.clear();
	}

public:
	/// Create a generator for the context structure.
	///
	/// \return A range for the context data structure.
	[[nodiscard]] auto getRangeContext() noexcept
	{
		auto scope = makeSyncSharedLockGuard(contextMutex_);
		return ranges::associateScope(context_, bzd::move(scope));
	}

private:
	/// Create a scope for the current context, it uses RAII pattern to control the lifetime of the context.
	///
	/// \param context The context object to be attached.
	/// \return A scope controlling the lifetime of this context.
	[[nodiscard]] auto registerContext(ExecutorContext<Executable>& context) noexcept
	{
		{
			auto scope = makeSyncLockGuard(contextMutex_);
			const auto result = context_.pushFront(context);
			bzd::assert::isTrue(result.hasValue());
		}

		{
			// Update the maximum of scheduler running concurrently.
			Size maybeMax, expected;
			do
			{
				maybeMax = context_.size();
				expected = maxRunningCount_.load();
			} while (maybeMax > expected && !maxRunningCount_.compareExchange(expected, maybeMax));
		}

		return ScopeGuard{[this, &context]() {
			auto scope = makeSyncLockGuard(contextMutex_);
			const auto result = context_.pop(context);
			if (context_.empty())
			{
				status_.store(Status::idle);
			}
			bzd::assert::isTrue(result.hasValue());
		}};
	}

	/// Create a scope for the current profiler, it uses RAII pattern to control the lifetime of the context.
	///
	/// \param profiler The profiler object to be attached.
	/// \param context The context object to be attached.
	/// \return A scope controlling the lifetime of the profiler.
	template <class Profiler>
	[[nodiscard]] auto registerProfiler(Profiler& profiler, const ExecutorContext<Executable>& context) const noexcept
	{
		const auto uid = context.getUId();
		profiler.event(profiler::NewCore{uid});
		return ScopeGuard{[uid, &profiler]() { profiler.event(profiler::DeleteCore{uid}); }};
	}

	/// Push a new workload to the queue.
	///
	/// It is pushed at the end of the queue, so will be executed after all previous workload are.
	///
	/// \param executable The workload to be executed.
	/// \param increment Increment the counters.
	constexpr void push(Executable& executable, const Bool increment = true) noexcept
	{
		if (increment)
		{
			// It is important to update the counters before being pushed, otherwise the executable might be
			// popped before the counters are increases, leaving them in an incoherent state.
			incrementCounters(executable);
		}
		// Only at the end push the executable to the work queue.
		queue_.pushBack(executable);
	}

	/// Unskip an executable already added to the queue.
	///
	/// This call is ISR friendly.
	constexpr void unskip(Executable& executable) noexcept
	{
		incrementCounters(executable);
		executable.unskip();
	}

	constexpr void incrementCounters(Executable& executable) noexcept
	{
		if (executable.getType() == ExecutableMetadata::Type::workload)
		{
			++workloadCount_;
		}
		++queueCount_;
	}

	/// Pop the next executable to process.
	///
	/// \param context The context that should match the executable, to ensure that async runs on the
	///                same thread.
	[[nodiscard]] bzd::Optional<Executable&> pop(ExecutorContext<Executable>& context) noexcept
	{
		const auto coreUId = context.getCoreUId();
		auto maybeExecutable = queue_.popFront([coreUId](auto& executable) { return executable.isReadyToBeScheduled(coreUId); });
		if (maybeExecutable)
		{
			// Show the stack usage
			// void* stack = __builtin_frame_address(0);
			// static void* initial_stack = &stack;
			// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPointer>(initial_stack) -
			// reinterpret_cast<bzd::IntPointer>(stack)) << std::endl;

			// Associate this executable with this context.
			maybeExecutable->pinCore(coreUId);

			if (maybeExecutable->getType() == ExecutableMetadata::Type::workload)
			{
				auto current = --workloadCount_;
				bzd::assert::isTrue(current >= 0);
			}
			--queueCount_;

			return maybeExecutable.valueMutable();
		}

		return bzd::nullopt;
	}

private:
	template <class U>
	friend class bzd::async::impl::Executable;
	template <class U>
	friend class bzd::async::impl::ExecutableSuspended;

	/// List of pending workload waiting to be scheduled.
	bzd::threadsafe::NonOwningRingSpin<Executable> queue_{};
	/// Keep contexts about the current running scheduler.
	bzd::threadsafe::NonOwningForwardList<ExecutorContext<Executable>> context_{};
	/// Mutex to protect access over the context queue.
	bzd::SpinSharedMutex contextMutex_{};
	/// Maximum concurrent scheduler running at the same time.
	bzd::Atomic<Size> maxRunningCount_{0u};
	/// Current status of the executor.
	bzd::Atomic<Status> status_{Status::idle};
	/// Number of entries in the queue.
	bzd::Atomic<Size> queueCount_{0u};
	/// Number of workload asyncs in the queue.
	/// Note getWorkloadCount() should never be negative, we use an int32 here only for testing purpose
	/// to avoid an infinite loop.
	bzd::Atomic<Int32> workloadCount_{0u};
};

} // namespace bzd::async::impl
