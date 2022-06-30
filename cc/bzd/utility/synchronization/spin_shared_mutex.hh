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

public: // API.
	// Locks the mutex, blocks if the mutex is not available.
	constexpr void lock() noexcept
	{
		UInt32 expected;
		do
		{
			expected = 0;
		} while (!lock_.compareExchange(expected, 1, MemoryOrder::acquire));
	}

	// Unlocks the mutex.
	constexpr void unlock() noexcept { lock_.store(0, MemoryOrder::release); }

	constexpr void lockShared() noexcept
	{
		UInt32 expected;
		do
		{
			expected = lock_.load(MemoryOrder::relaxed) & 0xffff0000;
		} while (!lock_.compareExchange(expected, expected + 0x10000, MemoryOrder::acquire));
	}

	// Unlocks the shared mutex.
	constexpr void unlockShared() noexcept { lock_ -= 0x10000; }

private:
	Atomic<UInt32> lock_{0};
};
} // namespace bzd
