#pragma once

#include "cc/bzd/container/function_view.hh"
#include "cc/bzd/container/impl/non_owning_list.hh"
#include "cc/bzd/core/async/coroutine.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd::inteface {
template <class T>
class Executable;
}

namespace bzd::concepts {
template <class T, class U>
concept executable = isBaseOf<bzd::inteface::Executable<U>, T>;
}

namespace bzd {

/// The executor concept is a workload scheduler that owns several executables
/// and executes them.
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
	constexpr void push(Executable& exectuable) noexcept { bzd::ignore = queue_.pushFront(exectuable); }

	/// Run all the workload currently in the queue.
	///
	/// Drain all workloads and exit only when the queue is empty.
	void run()
	{
		// Loop until there are still elements
		while (!queue_.empty())
		{
			// Drain remaining handles
			auto maybeExecutable = pop();
			if (maybeExecutable.hasValue())
			{
				maybeExecutable->resume();
			}
		}
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
};

} // namespace bzd

namespace bzd::interface {

/// Executable interface class.
///
/// This class provides helpers to access the associated executor.
///
/// \tparam T The child class, this is a CRTP desgin pattern.
template <class T>
class Executable : public bzd::NonOwningListElement</*multi container*/ true>
{
public:
	constexpr void enqueue() noexcept
	{
		bzd::assert::isTrue(executor_);
		executor_->push(*static_cast<T*>(this));
	}

	constexpr void setExecutor(bzd::Executor<T>& executor) noexcept { executor_ = &executor; }

	[[nodiscard]] constexpr BoolType hasExecutor() const noexcept {return (executor_ != nullptr); }

	constexpr bzd::Executor<T>* getExecutor() const noexcept {
		return executor_; // NOLINT(clang-analyzer-core.uninitialized.UndefReturn)
	}

private:
	bzd::Executor<T>* executor_{nullptr};
};

} // namespace bzd::interface
