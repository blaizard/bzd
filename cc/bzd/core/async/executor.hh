#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/range/associate_scope.hh"
#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/container/threadsafe/non_owning_queue_spin.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd {

template <class Executable>
class Executor;

namespace interface {
template <class T>
class Executable;
}

/// Metadata of an executable, propagated to all its children.
class ExecutableMetadata
{
public: // Types.
	/// The executable type can only be set once.
	enum class Type : bzd::UInt8
	{
		unset,
		/// Defines the type of executable that needs to complete.
		workload,
		/// Defines the type of executable that can be terminated if no
		/// workload executables are running.
		service
	};

public: // Accessors.
	[[nodiscard]] constexpr Type getType() const noexcept { return type_; }

private:
	template <class U>
	friend class bzd::interface::Executable;

	// Type of executable.
	Type type_{Type::unset};
};

/// Executor context.
template <class Executable>
class ExecutorContext : public bzd::threadsafe::NonOwningForwardListElement</*multi container*/ false>
{
public: // Traits.
	using Executor = bzd::Executor<Executable>;
	using IdType = UInt32;
	using TickType = UInt32;
	using OnTerminateCallback = bzd::FunctionRef<bzd::Optional<Executable&>(void)>;
	using Continuation = bzd::Variant<bzd::monostate, Executable*, OnTerminateCallback>;

public:
	constexpr ExecutorContext() noexcept : id_{makeUId()} {}

	/// Get the unique identifier of this context accross this executor.
	[[nodiscard]] constexpr IdType getUId() const noexcept { return id_; }
	/// Get the current tick for this executor.
	[[nodiscard]] constexpr TickType getTick() const noexcept { return tick_; }
	/// Provide an executable to be enqued sequentially, after the execution of the current exectuable.
	constexpr void setContinuation(Continuation&& continuation) noexcept
	{
		bzd::assert::isTrue(continuation_.template is<bzd::monostate>());
		continuation_ = bzd::move(continuation);
	}

private:
	friend class bzd::Executor<Executable>;

	constexpr void updateTick() noexcept { ++tick_; }

	[[nodiscard]] constexpr bzd::Optional<Executable&> popContinuation() noexcept
	{
		bzd::Optional<Executable&> maybeExecutable{};
		continuation_.match([](bzd::monostate) noexcept {},
							[&](Executable* executable) noexcept {
								if (executable)
								{
									maybeExecutable.emplace(*executable);
								}
							},
							[&](OnTerminateCallback callback) noexcept { maybeExecutable = callback(); });
		continuation_.template emplace<bzd::monostate>();
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
			if (maybeExecutable.hasValue())
			{
				auto& executable = maybeExecutable.valueMutable();
				if (executable.isCanceled())
				{
					executable.cancel(context);
				}
				else
				{
					executable.resume(context);
				}

				// Enqueue an executable if needed.
				auto maybeExecutableToEnqueue = context.popContinuation();
				if (maybeExecutableToEnqueue)
				{
					push(maybeExecutableToEnqueue.valueMutable());
				}
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

	// constexpr void waitToDiscard() noexcept { /*queue_.waitToDiscard();*/ }

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
		if (executable.getType() == ExecutableMetadata::Type::workload)
		{
			++workloadCount_;
		}
		++queueCount_;
		// Only at the end push the executable to the work queue.
		queue_.pushFront(executable);
	}

	[[nodiscard]] bzd::Optional<Executable&> pop() noexcept
	{
		auto maybeExecutable = queue_.popBack();
		if (maybeExecutable)
		{
			// Show the stack usage
			// void* stack = __builtin_frame_address(0);
			// static void* initial_stack = &stack;
			// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPointer>(initial_stack) -
			// reinterpret_cast<bzd::IntPointer>(stack)) << std::endl;

			if (maybeExecutable->getType() == ExecutableMetadata::Type::workload)
			{
				--workloadCount_;
			}
			--queueCount_;

			return maybeExecutable.valueMutable();
		}

		return bzd::nullopt;
	}

private:
	template <class U>
	friend class bzd::interface::Executable;

	/// List of pending workload waiting to be scheduled.
	bzd::threadsafe::NonOwningQueue<Executable> queue_{};
	/// Keep contexts about the current runnning scheduler.
	bzd::threadsafe::NonOwningForwardList<ExecutorContext<Executable>> context_{};
	/// Maxium concurrent scheduler running at the same time.
	bzd::Atomic<Size> maxRunningCount_{0u};
	/// Mutex to protect access over the context queue.
	bzd::SpinSharedMutex contextMutex_{};
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

namespace bzd::interface {

/// Executable interface class.
///
/// This class provides helpers to access the associated executor.
///
/// \tparam T The child class, this is a CRTP desgin pattern.
template <class T>
class Executable : public bzd::threadsafe::NonOwningQueueElement
{
public:
	using Self = Executable<T>;

public:
	~Executable() noexcept { destroy(); }

	constexpr void setCancellationToken(CancellationToken& token) noexcept
	{
		bzd::assert::isTrue(cancel_.empty());
		cancel_.emplace(token);
	}

	constexpr bzd::Optional<CancellationToken&> getCancellationToken() noexcept { return cancel_; }

	constexpr Bool isCanceled() const noexcept
	{
		if (cancel_.empty())
		{
			return false;
		}
		return cancel_->isCanceled();
	}

	constexpr bzd::Executor<T>& getExecutor() noexcept
	{
		bzd::assert::isTrue(executor_.hasValue());
		return executor_.valueMutable();
	}

	constexpr T& getExecutable() noexcept { return *static_cast<T*>(this); }

	[[nodiscard]] constexpr bzd::ExecutableMetadata::Type getType() const noexcept { return metadata_.getType(); }

	constexpr void setType(const bzd::ExecutableMetadata::Type type) noexcept
	{
		bzd::assert::isTrue(metadata_.getType() == bzd::ExecutableMetadata::Type::unset);
		metadata_.type_ = type;
	}

	/// Enqueue an executable to its executor. This assumes that an executor is already associated with this executable.
	constexpr void schedule() noexcept { getExecutor().push(getExecutable()); }

	constexpr void destroy() noexcept
	{
		assert::isTrue(isDetached(), "Executable still owned by the queue.");
		if (executor_.hasValue())
		{
			//  If there is an executor wait until it is safe to discard the element, this to avoid any
			//  potential out of scope object access.
			// bzd::ignore = executor_->pop(getExecutable());
			// executor_->waitToDiscard();
			executor_.reset();
		}
	}

	/// Propagate the context of this executable to another one.
	constexpr void propagate(T& executable) noexcept
	{
		// Propagate the cancellation token if any.
		executable.cancel_ = cancel_;
		// Propagate the executor.
		executable.executor_ = executor_;
		// Propagate the metadata.
		executable.metadata_ = metadata_;
	}

private:
	friend class bzd::Executor<T>;

	constexpr void setExecutor(bzd::Executor<T>& executor) noexcept { executor_.emplace(executor); }

	bzd::Optional<bzd::Executor<T>&> executor_{};
	bzd::Optional<CancellationToken&> cancel_{};
	bzd::ExecutableMetadata metadata_{};
};

} // namespace bzd::interface
