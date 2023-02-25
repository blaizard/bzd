#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/range/associate_scope.hh"
#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/container/threadsafe/non_owning_ring_spin.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/async/executable.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd {

template <class Executable>
class Executor;

/// Executor context.
template <class Executable>
class ExecutorContext : public bzd::threadsafe::NonOwningForwardListElement</*multi container*/ false>
{
public: // Traits.
	using Executor = bzd::Executor<Executable>;
	using IdType = UInt32;
	using TickType = UInt32;
	using OnTerminateCallback = bzd::FunctionRef<bzd::Optional<Executable&>(void)>;
	using Continuation = bzd::Variant<bzd::Monostate, Executable*, OnTerminateCallback>;

public:
	constexpr ExecutorContext() noexcept : id_{makeUId()} {}

	/// Get the unique identifier of this context accross this executor.
	[[nodiscard]] constexpr IdType getUId() const noexcept { return id_; }
	/// Get the current tick for this executor.
	[[nodiscard]] constexpr TickType getTick() const noexcept { return tick_; }
	/// Provide an executable to be enqued sequentially, after the execution of the current exectuable.
	constexpr void setContinuation(Continuation&& continuation) noexcept
	{
		bzd::assert::isTrue(continuation_.template is<bzd::Monostate>());
		continuation_ = bzd::move(continuation);
	}

private:
	friend class bzd::Executor<Executable>;

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
	const IdType id_;
	TickType tick_{0};
	Continuation continuation_{};
};

/// The executor concept is a workload scheduler that owns several executables
/// and executes them.
/// An executor is thread-safe and can be shared betweeen multiple threads or cores.
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
	void run() noexcept
	{
		// Storage for context related to the this running instance.
		ExecutorContext<Executable> context{};
		auto scope = registerContext(context);

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
			auto maybeExecutable = pop();
			while (maybeExecutable.hasValue())
			{
				auto& executable = maybeExecutable.valueMutable();
				const auto isCanceled = executable.isCanceled();
				if (isCanceled)
				{
					executable.cancel(context);
				}
				else
				{
					executable.resume(context);
				}

				// Execute the continuation if any, this instead of enqueuing it,
				// as it will speed up execution by avoiding unecessary atomic operations,
				// keeping continuation running on the same core to please caching
				// and reduce the number of spin locks required to update the queue.
				maybeExecutable = context.popContinuation();
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
	constexpr void schedule(Executable& executable, const bzd::ExecutableMetadata::Type type) noexcept
	{
		executable.setType(type);
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
		return range::associateScope(context_, bzd::move(scope));
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

	/// Push a new workload to the queue.
	///
	/// It is pushed at the end of the queue, so will be executed after all previous workload are.
	///
	/// \param executable The workload to be executed.
	constexpr void push(Executable& executable) noexcept
	{
		// It is important to update the counters before being pushed, otherwise the exectuable might be
		// popped before the counters are increases, leaving them in an incoherent state.
		incrementCounters(executable);
		// Only at the end push the executable to the work queue.
		queue_.pushBack(executable);
	}

	/// Push a new workload to the queue and mark it as skipped.
	///
	/// It will not be executed until the `unskip` call is performed.
	///
	/// \param executable The workload to be pushed and skipped.
	constexpr void pushSkip(Executable& executable) noexcept
	{
		executable.skip();
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

	[[nodiscard]] bzd::Optional<Executable&> pop() noexcept
	{
		auto maybeExecutable = queue_.popFront([](auto& exectuable) { return !exectuable.isSkipped(); });
		if (maybeExecutable)
		{
			// Show the stack usage
			// void* stack = __builtin_frame_address(0);
			// static void* initial_stack = &stack;
			// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPointer>(initial_stack) -
			// reinterpret_cast<bzd::IntPointer>(stack)) << std::endl;

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
	friend class bzd::interface::Executable;
	template <class U>
	friend class bzd::interface::ExecutableSuspendedForISR;

	/// List of pending workload waiting to be scheduled.
	bzd::threadsafe::NonOwningRingSpin<Executable> queue_{};
	/// Keep contexts about the current runnning scheduler.
	bzd::threadsafe::NonOwningForwardList<ExecutorContext<Executable>> context_{};
	/// Mutex to protect access over the context queue.
	bzd::SpinSharedMutex contextMutex_{};
	/// Maxium concurrent scheduler running at the same time.
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

} // namespace bzd
