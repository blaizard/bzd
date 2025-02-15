#pragma once

#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {
class SpinMutex
{
public: // Constructors/assignments.
	constexpr SpinMutex() = default;

	// Copy/move constructor/assignment.
	constexpr SpinMutex(const SpinMutex&) noexcept = delete;
	constexpr SpinMutex& operator=(const SpinMutex&) noexcept = delete;
	SpinMutex(SpinMutex&&) noexcept = delete;
	SpinMutex& operator=(SpinMutex&&) noexcept = delete;
	~SpinMutex() = default;

public: // API.
	constexpr void lock() noexcept { while (!tryLock()); }

	/// Tries to lock the mutex. Returns immediately.
	/// On successful lock acquisition returns true, otherwise returns false.
	constexpr Bool tryLock() noexcept { return !lock_.exchange(true, MemoryOrder::acquire); }

	constexpr void unlock() noexcept { lock_.store(false, MemoryOrder::release); }

private:
	Atomic<Bool> lock_{false};
};
} // namespace bzd
