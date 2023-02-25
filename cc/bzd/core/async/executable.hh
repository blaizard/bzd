#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/threadsafe/non_owning_ring_spin.hh"
#include "cc/bzd/core/async/cancellation.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

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
	[[nodiscard]] constexpr Bool skip() const noexcept { return flags_.load(MemoryOrder::relaxed) == Flags::skip; }

private:
	template <class U>
	friend class bzd::interface::Executable;

	constexpr void setSkip() noexcept { flags_.store(Flags::skip, MemoryOrder::relaxed); }
	constexpr void clearSkip() noexcept { flags_.store(Flags::none, MemoryOrder::relaxed); }

	// Type of executable.
	Type type_{Type::unset};
	// Flags associated with this executable.
	bzd::Atomic<Flags> flags_{Flags::none};
};

} // namespace bzd

namespace bzd::impl {

/// Factory to store an executable when suspended.
template <class T, class Impl>
class ExecutableSuspendedFactory
{
public: // Constructors/destructor/...
	constexpr ExecutableSuspendedFactory() noexcept : executable_{nullptr}, onCancel_{makeCallback()}, onUserCancel_{onCancel_} {}
	constexpr ExecutableSuspendedFactory(T& executable, const bzd::FunctionRef<void(void)> onCancel) noexcept :
		executable_{&executable}, onCancel_{makeCallback()}, onUserCancel_{onCancel}
	{
		if (executable.cancel_)
		{
			executable.cancel_->addOneTimeCallback(onCancel_);
		}
	}
	ExecutableSuspendedFactory(const ExecutableSuspendedFactory&) = delete;
	ExecutableSuspendedFactory& operator=(const ExecutableSuspendedFactory&) = delete;
	constexpr ExecutableSuspendedFactory(ExecutableSuspendedFactory&& other) noexcept : ExecutableSuspendedFactory{}
	{
		*this = bzd::move(other);
	}
	constexpr ExecutableSuspendedFactory& operator=(ExecutableSuspendedFactory&& other) noexcept
	{
		// Do not allow move from a non-empty container.
		bzd::assert::isTrue(executable_.load() == nullptr);

		auto* executable = other.executable_.exchange(processing);
		if (executable != processing)
		{
			if (executable)
			{
				// Store the executble now, in that case, if a race happen with a cancellation,
				// nothing will be done until addOneTimeCallback is invoked.
				executable_.store(executable);
				onUserCancel_ = bzd::move(other.onUserCancel_);
				if (executable->cancel_)
				{
					executable->cancel_->removeCallback(other.onCancel_);
					executable->cancel_->addOneTimeCallback(onCancel_);
				}
			}
			other.executable_.store(nullptr);
		}

		return *this;
	}
	constexpr ~ExecutableSuspendedFactory() noexcept
	{
		// There can be a cancellation going on, so wait until it is completly done.
		while (executable_.load() == processing)
		{
		};
		bzd::assert::isTrue(!executable_.load(), "Suspended was destroyed with an executable.");
	}

public: // API.
	constexpr void schedule() noexcept
	{
		auto* executable = executable_.exchange(processing);
		if (executable != processing)
		{
			if (executable)
			{
				if (executable->cancel_)
				{
					executable->cancel_->removeCallback(onCancel_);
				}
				Impl::reschedule(*executable);
			}
			executable_.store(nullptr);
		}
	}

private:
	constexpr void cancel() noexcept
	{
		auto* executable = executable_.exchange(processing);
		if (executable != processing)
		{
			if (executable)
			{
				onUserCancel_();
				Impl::reschedule(*executable);
			}
			executable_.store(nullptr);
		}
	}

	auto makeCallback()
	{
		return bzd::FunctionRef<void(void)>::toMember<ExecutableSuspendedFactory, &ExecutableSuspendedFactory::cancel>(*this);
	}

private:
	// Special value to denote that scheduling/cancellation is on-going.
	static inline T* processing{reinterpret_cast<T*>(1)};
	bzd::Atomic<T*> executable_;
	bzd::CancellationCallback onCancel_;
	bzd::FunctionRef<void(void)> onUserCancel_;
};

} // namespace bzd::impl

namespace bzd::interface {

/// Type to store an executable when suspended.
///
/// This type deals with a suspended executable that is poped from the executor queue.
/// It should be used as the prefered method as the suspended executable will not affect
/// the queue as it is completly removed.
template <class T>
class ExecutableSuspended : public bzd::impl::ExecutableSuspendedFactory<T, ExecutableSuspended<T>>
{
public:
	using bzd::impl::ExecutableSuspendedFactory<T, ExecutableSuspended<T>>::ExecutableSuspendedFactory;

private:
	friend class bzd::impl::ExecutableSuspendedFactory<T, ExecutableSuspended<T>>;
	static constexpr void reschedule(T& executable) noexcept { executable.reschedule(); }
};

/// Type to store an executable when suspended for use with ISR (aka in wait-free context).
template <class T>
class ExecutableSuspendedForISR : public bzd::impl::ExecutableSuspendedFactory<T, ExecutableSuspendedForISR<T>>
{
public:
	using bzd::impl::ExecutableSuspendedFactory<T, ExecutableSuspendedForISR<T>>::ExecutableSuspendedFactory;

private:
	friend class bzd::impl::ExecutableSuspendedFactory<T, ExecutableSuspendedForISR<T>>;
	static constexpr void reschedule(T& executable) noexcept { executable.clearSkip(); }
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

	[[nodiscard]] constexpr Bool skip() const noexcept { return metadata_.skip(); }
	constexpr void clearSkip() noexcept { metadata_.clearSkip(); }
	constexpr void setSkip() noexcept { metadata_.setSkip(); }

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

	constexpr auto suspend(const bzd::FunctionRef<void(void)> onCancel) noexcept
	{
		return bzd::interface::ExecutableSuspended<T> {
			getExecutable(), onCancel
		};
	}

	constexpr auto suspendForISR(const bzd::FunctionRef<void(void)> onCancel) noexcept
	{
		setSkip();
		reschedule();
		return bzd::interface::ExecutableSuspendedForISR<T> {
			getExecutable(), onCancel
		};
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
	template <class, class>
	friend class bzd::impl::ExecutableSuspendedFactory;
	friend class bzd::interface::ExecutableSuspended<T>;
	friend struct bzd::coroutine::impl::Yield;

	constexpr void setExecutor(bzd::Executor<T>& executor) noexcept { executor_.emplace(executor); }
	constexpr void reschedule() noexcept { getExecutor().push(getExecutable()); }

	bzd::Optional<bzd::Executor<T>&> executor_{};
	bzd::Optional<CancellationToken&> cancel_{};
	bzd::ExecutableMetadata metadata_{};
};

} // namespace bzd::interface
