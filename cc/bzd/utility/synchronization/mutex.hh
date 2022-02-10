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
		while (acquired_.exchange(true))
		{
			co_await bzd::async::yield();
		}
		co_return {};
	}

	constexpr void unlock() noexcept { acquired_.store(false); }

private:
	bzd::Atomic<bzd::BoolType> acquired_{false};
};

} // namespace bzd