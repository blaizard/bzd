#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/threadsafe/non_owning_ring_spin.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

#include <iostream>

namespace bzd {
template <class Executable>
class Executor;

namespace interface {
template <class T>
class Executable;
} // namespace interface

namespace coroutine::impl {
struct Yield;
}

/// Metadata of an executable, propagated to all its children.
///
/// All flags are set atomically, but with no memory fencing guarantee, the reader should
/// therefore take care of the fencing if required.
class ExecutableMetadata
{
public: // Constructors/destructor/...
	ExecutableMetadata() = default;
	constexpr ExecutableMetadata(const ExecutableMetadata& other) noexcept { *this = other; }
	constexpr ExecutableMetadata& operator=(const ExecutableMetadata& other) noexcept
	{
		type_ = other.type_;
		flags_.store(other.flags_.load(MemoryOrder::relaxed), MemoryOrder::relaxed);
		return *this;
	}
	ExecutableMetadata(ExecutableMetadata&&) = delete;
	ExecutableMetadata& operator=(ExecutableMetadata&&) = delete;
	~ExecutableMetadata() = default;

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

	/// Contains the flags associated with the executbale.
	enum class Flags : bzd::UInt8
	{
		none = 0,
		/// If the executable needs to be skipped from the executor queue.
		skip = 1
	};

public: // Accessors.
	[[nodiscard]] constexpr Type getType() const noexcept { return type_; }
	[[nodiscard]] constexpr Bool isSkipped() const noexcept { return flags_.load(MemoryOrder::acquire) == Flags::skip; }

private:
	template <class U>
	friend class bzd::interface::Executable;

	constexpr void skip() noexcept { flags_.store(Flags::skip, MemoryOrder::release); }
	constexpr void unskip() noexcept { flags_.store(Flags::none, MemoryOrder::release); }

	// Type of executable.
	Type type_{Type::unset};
	// Flags associated with this executable.
	bzd::Atomic<Flags> flags_{Flags::none};
};

} // namespace bzd

namespace bzd::interface {

/// Type to store an executable when suspended for use with ISR (aka in wait-free context).
template <class T>
class ExecutableSuspended
{
public: // Constructors/destructor/...
	constexpr ExecutableSuspended() noexcept : executable_{nullptr}, onCancel_{makeCallback()}, onUserCancel_{onCancel_} {}
	ExecutableSuspended(const ExecutableSuspended&) = delete;
	ExecutableSuspended& operator=(const ExecutableSuspended&) = delete;
	ExecutableSuspended& operator=(ExecutableSuspended&&) = delete;
	constexpr ExecutableSuspended(ExecutableSuspended&& other) noexcept : ExecutableSuspended{} { own(bzd::move(other)); }
	constexpr ~ExecutableSuspended() noexcept { bzd::assert::isTrue(!executable_.load(), "Destroyed with an executable."); }

public: // API.
	/// Become the owner of another ExecutableSuspended.
	///
	/// The current object must be empty.
	/// This function cannot be used after activated.
	///
	/// @param other The new ExecutableSuspended to be owned.
	constexpr void own(ExecutableSuspended&& other) noexcept
	{
		bzd::assert::isTrue(executable_.load() == nullptr);
		// ExecutableSuspended that are owned must have an owner, if not it means that they are
		// already activated. We cannot own an ExecutableSuspended after being activated.
		bzd::assert::isTrue(other.owner_);
		bzd::assert::isTrue(*(other.owner_));

		auto* executable = other.executable_.exchange(nullptr);
		if (executable)
		{
			owner_ = other.owner_;
			*owner_ = this;
			onUserCancel_ = other.onUserCancel_;
			executable_.store(executable);
		}
	}

	/// Reschedule the suspended executable owned by this object.
	constexpr Bool schedule() noexcept
	{
		auto* executable = executable_.exchange(nullptr);
		if (executable)
		{
			if (executable->cancel_)
			{
				executable->cancel_->removeCallback(onCancel_);
			}
			unskip(*executable);
			return true;
		}

		return false;
	}

private:
	constexpr void cancel() noexcept
	{
		auto* executable = executable_.exchange(nullptr);
		if (executable)
		{
			onUserCancel_();
			unskip(*executable);
		}
	}

	auto makeCallback() { return bzd::FunctionRef<void(void)>::toMember<ExecutableSuspended, &ExecutableSuspended::cancel>(*this); }

	constexpr void unskip(T& executable) noexcept { executable.getExecutor().unskip(executable); }

protected:
	/// Enable the executable by adding the cancellation callback
	/// and rescheduling it.
	///
	/// After this call, the ExecutableSuspended cannot be moved anymore.
	///
	/// \param executable The executable to be activated.
	constexpr void activate(T& executable) noexcept
	{
		bzd::assert::isTrue(owner_);
		bzd::assert::isTrue(*owner_);
		if (executable.cancel_)
		{
			auto& owner = **owner_;
			executable.cancel_->addOneTimeCallback(owner.onCancel_);
			// After adding the callback, check if a reschedule was triggered,
			// if so, the cancellation might not have been removed if it was done
			// before the previous statement.
			if (!owner.executable_.load())
			{
				executable.cancel_->removeCallback(owner.onCancel_);
			}
		}
		// Make sure the executable cannot be activated twice.
		*owner_ = nullptr;
		// Only after this statement, the executable might not be valid (it might
		// be executed already by a concurrent thread).
		executable.reschedule(/*increment*/ false);
	}

protected:
	bzd::Atomic<T*> executable_;
	bzd::CancellationCallback onCancel_;
	bzd::FunctionRef<void(void)> onUserCancel_;
	ExecutableSuspended** owner_{nullptr};
};

/// Executable interface class.
///
/// This class provides helpers to access the associated executor.
///
/// \tparam T The child class, this is a CRTP desgin pattern.
template <class T>
class Executable : public bzd::threadsafe::NonOwningRingSpinElement
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

	[[nodiscard]] constexpr Bool isSkipped() const noexcept { return metadata_.isSkipped(); }
	constexpr void skip() noexcept { metadata_.skip(); }

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

	constexpr void destroy() noexcept
	{
		assert::isTrue(isDetached(), "Executable still owned by the queue.");
		if (executor_.hasValue())
		{
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
	friend class bzd::interface::ExecutableSuspended<T>;
	friend struct bzd::coroutine::impl::Yield;

	constexpr void setExecutor(bzd::Executor<T>& executor) noexcept { executor_.emplace(executor); }
	/// Reschedule the async in the executor.
	///
	/// \param increment Increment the internal counters.
	constexpr void reschedule(const Bool increment = true) noexcept { getExecutor().push(getExecutable(), increment); }
	constexpr void unskip() noexcept { metadata_.unskip(); }

	bzd::Optional<bzd::Executor<T>&> executor_{};
	bzd::Optional<CancellationToken&> cancel_{};
	bzd::ExecutableMetadata metadata_{};
};

} // namespace bzd::interface
