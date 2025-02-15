#pragma once

#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {
/// In contrast to other mutex types which facilitate exclusive access, a shared_mutex has two levels of access:
/// - shared: several threads can share ownership of the same mutex.
/// - exclusive: only one thread can own the mutex.
class SpinSharedMutex
{
public: // Constructors/assignments.
	constexpr SpinSharedMutex() = default;

	// Copy/move constructor/assignment.
	constexpr SpinSharedMutex(const SpinSharedMutex&) noexcept = delete;
	constexpr SpinSharedMutex& operator=(const SpinSharedMutex&) noexcept = delete;
	SpinSharedMutex(SpinSharedMutex&&) noexcept = delete;
	SpinSharedMutex& operator=(SpinSharedMutex&&) noexcept = delete;
	~SpinSharedMutex() = default;

public: // API.
	/// Locks the mutex, blocks if the mutex is not available.
	constexpr void lock() noexcept { while (!tryLock()); }

	/// Tries to lock the mutex. Returns immediately.
	/// On successful lock acquisition returns true, otherwise returns false.
	constexpr Bool tryLock() noexcept
	{
		UInt32 expected{0u};
		return lock_.compareExchange(expected, lockValue, MemoryOrder::acquire);
	}

	/// Unlocks the mutex.
	constexpr void unlock() noexcept { lock_.store(0u, MemoryOrder::release); }

	constexpr void lockShared() noexcept { while (!tryLockShared()); }

	/// Tries to lock the mutex. Returns immediately.
	/// On successful lock acquisition returns true, otherwise returns false.
	constexpr Bool tryLockShared() noexcept
	{
		UInt32 expected;
		do
		{
			expected = lock_.load(MemoryOrder::relaxed) & lockSharedMask;
			if (lock_.compareExchange(expected, expected + 1u, MemoryOrder::acquire))
			{
				return true;
			}
		} while ((expected & lockValue) != lockValue);
		return false;
	}

	// Unlocks the shared mutex.
	constexpr void unlockShared() noexcept { --lock_; }

private:
	static constexpr UInt32 lockValue = 0x80000000;
	static constexpr UInt32 lockSharedMask = 0x7fffffff;
	Atomic<UInt32> lock_{0u};
};
} // namespace bzd
