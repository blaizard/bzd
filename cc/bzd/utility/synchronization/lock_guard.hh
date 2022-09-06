#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/type_traits/lockable.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd::lockGuard {
template <concepts::basicLockable T>
auto makeLambda(T& lock)
{
	auto scope = bzd::ScopeGuard([&lock]() { lock.unlock(); });
	return bzd::move(scope);
}

template <concepts::basicLockable T>
using Type = bzd::typeTraits::InvokeResult<decltype(makeLambda<T>), T&>;
} // namespace bzd::lockGuard

namespace bzd::sharedLockGuard {
template <concepts::sharedLockable T>
auto makeLambda(T& lock)
{
	auto scope = bzd::ScopeGuard([&lock]() { lock.unlockShared(); });
	return bzd::move(scope);
}

template <concepts::sharedLockable T>
using Type = bzd::typeTraits::InvokeResult<decltype(makeLambda<T>), T&>;
} // namespace bzd::sharedLockGuard

namespace bzd {
template <concepts::basicLockable T>
[[nodiscard]] auto makeLockGuard(T& lock) -> bzd::Async<lockGuard::Type<T>>
{
	co_await !lock.lock();
	auto scope = lockGuard::makeLambda(lock);
	co_return bzd::move(scope);
}

template <concepts::sharedLockable T>
[[nodiscard]] auto makeSharedLockGuard(T& lock) -> bzd::Async<sharedLockGuard::Type<T>>
{
	co_await !lock.lockShared();
	auto scope = sharedLockGuard::makeLambda(lock);
	co_return bzd::move(scope);
}
} // namespace bzd
