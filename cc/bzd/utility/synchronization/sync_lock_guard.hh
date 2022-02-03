#pragma once

#include "cc/bzd/type_traits/lockable.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd {
template <concepts::basicLockable T>
[[nodiscard]] constexpr auto makeSyncLockGuard(T& lock)
{
	lock.lock();
	return bzd::ScopeGuard([&lock]() { lock.unlock(); });
}

template <concepts::sharedLockable T>
[[nodiscard]] constexpr auto makeSyncSharedLockGuard(T& lock)
{
	lock.lockShared();
	return bzd::ScopeGuard([&lock]() { lock.unlockShared(); });
}
} // namespace bzd
