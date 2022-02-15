#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/function_view.hh"
#include "cc/bzd/container/impl/non_owning_list.hh"
#include "cc/bzd/container/range/associate_scope.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

#include <iostream>
#include <type_traits>

namespace bzd {
/// The executor concept is a workload scheduler that owns several executables
/// and executes them.
/// An executor is thread-safe and can be shared betweeen multiple threads or cores.
template <class Executable>
class Executor
{
public: // Traits.
	using Self = Executor<Executable>;
	using TickType = UInt32Type;

public:
	constexpr Executor() = default;

	// Copy/move constructor/assignment
	constexpr Executor(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr Executor(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

	constexpr ~Executor()
	{
		// Clear the complete queue.
		queue_.clear();
	}

public: // Statistics.
	[[nodiscard]] constexpr SizeType getQueueCount() const noexcept { return queue_.size(); }
	[[nodiscard]] constexpr SizeType getRunningCount() const noexcept { return running_.size(); }
	[[nodiscard]] constexpr SizeType getMaxRunningCount() const noexcept { return maxRunningCount_.load(); }

public:
	/// A unique counter with no units that increments after each scheduling.
	[[nodiscard]] static TickType getNextTick() noexcept
	{
		static Atomic<TickType> tick{0};

		// Increase the counter by ensuring the value zero is never set.
		// This is a special value that is used to avoid dead lock.
		TickType current;
		do
		{
			current = ++tick;
		} while (current == 0);
		return current;
	}

	/// Push a new workload to the queue.
	///
	/// It is pushed at the end of the queue, so will be executed after all previous workload are.
	///
	/// \param executable The workload to be executed.
	constexpr void push(Executable& exectuable) noexcept { bzd::ignore = queue_.pushFront(exectuable); }

	/// Run all the workload currently in the queue.
	///
	/// Drain all workloads and exit only when the queue is empty.
	void run()
	{
		// Storage for information related to the this running instance.
		RunningInfo info{};
		auto scope = registerInfo(info);

		// Loop until there are still elements
		while (!queue_.empty())
		{
			// Drain remaining handles
			auto maybeExecutable = pop();
			if (maybeExecutable.hasValue())
			{
				auto& executable = maybeExecutable.valueMutable();
				// On cancellation, ignore the executable and flag it as canceled.
				if (executable.isCanceledOrTriggered())
				{
					executable.cancel();
				}
				else
				{
					executable.resume();
				}
			}
			info.updateTick();
		}

		// Use to prevent a potential dead lock here if waitForNextTick is called at this moment
		// in another thread, scope will not be able to acquire the lock and the tick will
		// remain the same.
		info.markAsCompleted();
	}

private:
	class RunningInfo : public bzd::NonOwningListElement</*multi container*/ false>
	{
	public: // Traits.
		using IdType = UInt32Type;

	public:
		constexpr RunningInfo() noexcept : id_{makeUId()} {}

		[[nodiscard]] constexpr IdType getUId() const noexcept { return id_; }
		[[nodiscard]] constexpr TickType getTick() const noexcept { return tick_.load(); }

		constexpr void updateTick() noexcept { tick_.store(Executor::getNextTick()); }
		constexpr void markAsCompleted() noexcept { tick_.store(0); }
		constexpr auto isCompleted() const noexcept { return (tick_.load() == 0); }

	private:
		[[nodiscard]] static IdType makeUId() noexcept
		{
			static Atomic<IdType> id{0};
			return ++id;
		}

	private:
		const IdType id_;
		Atomic<TickType> tick_{0};
	};

public:
	/// Create a generator for the running structure.
	///
	/// \return A range for the running data structure.
	[[nodiscard]] auto getRangeRunning() noexcept
	{
		auto scope = makeSyncSharedLockGuard(runningMutex_);
		return range::associateScope(running_, bzd::move(scope));
	}

private:
	/// Create a scope for the running info, it uses RAII pattern to control the lifetime of the info.
	///
	/// \param info The information structure to be attached.
	/// \return A scope controlling the lifetime of this information.
	[[nodiscard]] auto registerInfo(RunningInfo& info) noexcept
	{
		{
			auto scope = makeSyncLockGuard(runningMutex_);
			const auto result = running_.pushFront(info);
			bzd::assert::isTrue(result.hasValue());
		}

		{
			// Update the maximum of scheduler running concurrently.
			SizeType maybeMax, expected;
			do
			{
				maybeMax = running_.size();
				expected = maxRunningCount_.load();
			} while (maybeMax > expected && !maxRunningCount_.compareExchange(expected, maybeMax));
		}

		return ScopeGuard{[this, &info]() {
			auto scope = makeSyncLockGuard(runningMutex_);
			const auto result = running_.pop(info);
			bzd::assert::isTrue(result.hasValue());
		}};
	}

	[[nodiscard]] bzd::Optional<Executable&> pop() noexcept
	{
		auto executable = queue_.back();
		if (executable)
		{
			const auto result = queue_.pop(executable.valueMutable());
			if (result)
			{
				// Show the stack usage
				// void* stack = __builtin_frame_address(0);
				// static void* initial_stack = &stack;
				// std::cout << "stack: "  << initial_stack << "+" << (reinterpret_cast<bzd::IntPtrType>(initial_stack) -
				// reinterpret_cast<bzd::IntPtrType>(stack)) << std::endl;

				return executable.valueMutable();
			}
		}

		return bzd::nullopt;
	}

private:
	/// List of pending workload waiting to be scheduled.
	bzd::NonOwningList<Executable> queue_{};
	/// Keep information about the current runnning scheduler.
	bzd::NonOwningList<RunningInfo> running_{};
	/// Maxium concurrent scheduler running at the same time.
	bzd::Atomic<SizeType> maxRunningCount_{0};
	/// Mutex to protect access over the running queue.
	bzd::SpinSharedMutex runningMutex_{};
};

} // namespace bzd

namespace bzd::interface {

class CancellationToken
{
public:
	constexpr CancellationToken(bzd::Atomic<UInt8Type>& flag) : flag_{flag} {}

	constexpr void trigger() noexcept
	{
		UInt8Type expected{0};
		flag_.compareExchange(expected, 1);
	}

	constexpr void cancel() noexcept { flag_.store(2); }

	constexpr BoolType isCanceledOrTriggered() const noexcept { return (flag_.load() != 0); }

	constexpr BoolType isCanceled() const noexcept { return (flag_.load() == 2); }

private:
	bzd::Atomic<UInt8Type>& flag_;
};

/// Executable interface class.
///
/// This class provides helpers to access the associated executor.
///
/// \tparam T The child class, this is a CRTP desgin pattern.
template <class T>
class Executable : public bzd::NonOwningListElement</*multi container*/ true>
{
public:
	using Self = Executable<T>;

public:
	~Executable()
	{
		// Detach the executable from any of the list it belongs to.
		bzd::ignore = pop();
	}

	constexpr void enqueue() noexcept
	{
		bzd::assert::isTrue(executor_);
		executor_->push(*static_cast<T*>(this));
	}

	constexpr void setExecutor(bzd::Executor<T>& executor) noexcept
	{
		bzd::assert::isTrue(!executor_);
		executor_ = &executor;
	}

	constexpr bzd::Executor<T>& getExecutor() const noexcept
	{
		// NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
		bzd::assert::isTrue(executor_);
		return *executor_;
	}

	constexpr void setCancellationToken(CancellationToken& token) noexcept
	{
		bzd::assert::isTrue(!cancel_);
		cancel_ = &token;
	}

	constexpr void trigger() noexcept { cancel_->trigger(); }

	constexpr void cancel() noexcept { cancel_->cancel(); }

	constexpr BoolType isCanceledOrTriggered() const noexcept
	{
		if (!cancel_)
		{
			return false;
		}
		return cancel_->isCanceledOrTriggered();
	}

	constexpr BoolType isCanceled() const noexcept
	{
		if (!cancel_)
		{
			return false;
		}
		return cancel_->isCanceled();
	}

	/// Propagate the current context to the executable passed into argument.
	///
	/// \param[out] executable The executable to receive the new context.
	constexpr void propagateContextTo(Self& executable) const noexcept
	{
		// NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign)
		executable.executor_ = executor_;
		bzd::assert::isTrue(!executable.cancel_);
		executable.cancel_ = cancel_;
	}

private:
	bzd::Executor<T>* executor_{nullptr};
	CancellationToken* cancel_{nullptr};
};

} // namespace bzd::interface
