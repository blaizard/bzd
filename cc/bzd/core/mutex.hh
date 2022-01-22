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
		bzd::BoolType expected{false};
		while (!acquired_.compareExchange(expected, true))
		{
			co_await bzd::async::yield();
			expected = false;
		}
		co_return {};
	}

	constexpr void unlock() noexcept { acquired_.store(false); }

private:
	bzd::Atomic<bzd::BoolType> acquired_{false};
};

} // namespace bzd
