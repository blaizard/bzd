#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/type_traits/lockable.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd::impl::lockGuard {
template <concepts::basicLockable T>
auto makeLambda(T& lock)
{
	auto scope = bzd::ScopeGuard([&lock]() { lock.unlock(); });
	return bzd::move(scope);
}
} // namespace bzd::impl::lockGuard

namespace bzd::impl::sharedLockGuard {
template <concepts::sharedLockable T>
auto makeLambda(T& lock)
{
	auto scope = bzd::ScopeGuard([&lock]() { lock.unlockShared(); });
	return bzd::move(scope);
}
} // namespace bzd::impl::sharedLockGuard

namespace bzd {
template <concepts::basicLockable T>
[[nodiscard]] auto makeLockGuard(T& lock) -> bzd::Async<bzd::typeTraits::InvokeResult<decltype(impl::lockGuard::makeLambda<T>), T&>>
{
	co_await lock.lock().assert();
	auto scope = impl::lockGuard::makeLambda(lock);
	co_return bzd::move(scope);
}

template <concepts::sharedLockable T>
[[nodiscard]] auto makeSharedLockGuard(T& lock)
	-> bzd::Async<bzd::typeTraits::InvokeResult<decltype(impl::sharedLockGuard::makeLambda<T>), T&>>
{
	co_await lock.lockShared().assert();
	auto scope = impl::sharedLockGuard::makeLambda(lock);
	co_return bzd::move(scope);
}
} // namespace bzd
