#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/function_view.hh"
#include "cc/bzd/container/impl/non_owning_list.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/utility/ignore.hh"

#include <iostream>
#include <type_traits>

namespace bzd {

/// The executor concept is a workload scheduler that owns several executables
/// and executes them.
/// An executor is thread-safe and can be shared betweeen multiple threads or cores.
template <class Executable>
class Executor
{
public:
	constexpr Executor() = default;

	/// Push a new workload to the queue.
	///
	/// It is pushed at the end of the queue, so will be executed after all previous workload are.
	///
	/// \param executable The workload to be executed.
	constexpr void push(Executable& exectuable) noexcept
	{
		// bzd::ignore = running_.pop(exectuable);
		bzd::ignore = queue_.pushFront(exectuable);
	}

	/// Run all the workload currently in the queue.
	///
	/// Drain all workloads and exit only when the queue is empty.
	void run()
	{
		++countActive_;
		// Look for the first free identifier
		SizeType identifier{0};
		for (auto& pc : pc_)
		{
			SizeType expected{0};
			if (pc.compareExchange(expected, 1))
			{
				break;
			}
			++identifier;
		}
		bzd::assert::isTrue(identifier < pc_.size(), "Too many executors run with this instance.");
		auto& pc = pc_[identifier];

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
					// bzd::ignore = running_.pushFront(executable);
					executable.resume();
				}
			}
			++pc;
		}

		// Free this identifier
		pc.store(0);
		--countActive_;
	}

private:
	bzd::Optional<Executable&> pop() noexcept
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
	bzd::NonOwningList<Executable> queue_{};
	// bzd::NonOwningList<Executable> running_{};
	/// The number of instances using this executor and currently running.
	bzd::Atomic<SizeType> countActive_{};
	/// Program counter for the different active executors running with this instance.
	bzd::Array<bzd::Atomic<SizeType>, 10> pc_{};
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
