#pragma once

#include "cc/bzd/container/named_type.hh"

#include <atomic>

namespace bzd {

enum class MemoryOrder
{
	/// Relaxed operation: there are no synchronization or ordering constraints imposed on other reads or writes,
	/// only this operation's atomicity is guaranteed.
	relaxed = static_cast<int>(std::memory_order_relaxed),
	/// A load operation with this memory order performs a consume operation on the affected memory location: no reads
	/// or writes in the current thread dependent on the value currently loaded can be reordered before this load.
	/// Writes to data-dependent variables in other threads that release the same atomic variable are visible in the
	/// current thread. On most platforms, this affects compiler optimizations only.
	consume = static_cast<int>(std::memory_order_consume),
	/// A load operation with this memory order performs the acquire operation on the affected memory location:
	/// no reads or writes in the current thread can be reordered before this load. All writes in other threads that
	/// release the same atomic variable are visible in the current thread.
	acquire = static_cast<int>(std::memory_order_acquire),
	/// A store operation with this memory order performs the release operation: no reads or writes in the current
	/// thread can be reordered after this store. All writes in the current thread are visible in other threads that
	/// acquire the same atomic variable and writes that carry a dependency into the atomic variable become visible
	/// in other threads that consume the same atomic.
	release = static_cast<int>(std::memory_order_release),
	/// A read-modify-write operation with this memory order is both an acquire operation and a release operation.
	/// No memory reads or writes in the current thread can be reordered before or after this store. All writes in other
	/// threads that release the same atomic variable are visible before the modification and the modification is
	/// visible in other threads that acquire the same atomic variable.
	acquireRelease = static_cast<int>(std::memory_order_acq_rel),
	/// A load operation with this memory order performs an acquire operation, a store performs a release operation,
	/// and read-modify-write performs both an acquire operation and a release operation, plus a single total order
	/// exists in which all threads observe all modifications in the same order.
	sequentiallyConsistent = static_cast<int>(std::memory_order_seq_cst),
};

template <class T>
class Atomic : public bzd::NamedType<std::atomic<T>, struct AtomicTag, bzd::Comparable, bzd::Incrementable, bzd::Decrementable>
{
public:
	using bzd::NamedType<std::atomic<T>, struct AtomicTag, bzd::Comparable, bzd::Incrementable, bzd::Decrementable>::NamedType;

	constexpr explicit Atomic(const T& value) noexcept { this->get() = value; }

	constexpr void store(const T& v, const MemoryOrder order = MemoryOrder::sequentiallyConsistent) noexcept
	{
		this->get().store(v, static_cast<std::memory_order>(order));
	}

	constexpr T load(const MemoryOrder order = MemoryOrder::sequentiallyConsistent) const noexcept
	{
		return this->get().load(static_cast<std::memory_order>(order));
	}

	constexpr T operator+=(const T& value) noexcept { return this->get().fetch_add(value); }
	constexpr T operator-=(const T& value) noexcept { return this->get().fetch_sub(value); }
	constexpr T operator&=(const T& value) noexcept { return this->get().fetch_and(value); }
	constexpr T operator|=(const T& value) noexcept { return this->get().fetch_or(value); }
	constexpr T operator^=(const T& value) noexcept { return this->get().fetch_xor(value); }
	constexpr explicit operator bool() const noexcept { return static_cast<bool>(this->get()); }

	constexpr T operator--() noexcept { return --this->get(); }
	constexpr T operator--(int) noexcept { return this->get()--; }

	constexpr T operator++() noexcept { return ++this->get(); }
	constexpr T operator++(int) noexcept { return this->get()++; }

	constexpr T exchange(T v, const MemoryOrder order = MemoryOrder::sequentiallyConsistent) noexcept
	{
		return this->get().exchange(v, static_cast<std::memory_order>(order));
	}

	constexpr bool compareExchange(T& expected, T desired, const MemoryOrder order = MemoryOrder::sequentiallyConsistent) noexcept
	{
		return this->get().compare_exchange_strong(expected, desired, static_cast<std::memory_order>(order));
	}
};

/// Establishes memory synchronization ordering.
constexpr void memoryFence(const MemoryOrder order) noexcept
{
	std::atomic_thread_fence(static_cast<std::memory_order>(order));
}

} // namespace bzd
