#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/atomic.hh"

namespace bzd {

class Mutex
{
public:
	constexpr Mutex() = default;

	// Copy/move constructor/assignment.
	constexpr Mutex(const Mutex& mutex) noexcept = delete;
	constexpr Mutex& operator=(const Mutex& mutex) noexcept = delete;
	Mutex(Mutex&& mutex) noexcept = delete;
	Mutex& operator=(Mutex&& mutex) noexcept = delete;

public:
	bzd::Async<> lock() noexcept
	{
		while (!tryLock())
		{
			co_await bzd::async::yield();
		}
		co_return {};
	}

	/// Tries to lock the mutex. Returns immediately.
	/// On successful lock acquisition returns true, otherwise returns false.
	constexpr Bool tryLock() noexcept { return !acquired_.exchange(true); }

	constexpr void unlock() noexcept { acquired_.store(false); }

private:
	bzd::Atomic<bzd::Bool> acquired_{false};
};

} // namespace bzd
