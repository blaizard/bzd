#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd::impl::lockGuard {
template <class T>
auto makeLambda(T& lock)
{
	auto scope = bzd::ScopeGuard([&lock]() { lock.unlock(); });
	return bzd::move(scope);
}
} // namespace bzd::impl::lockGuard

namespace bzd {
template <class T>
[[nodiscard]] auto makeLockGuard(T& lock) -> bzd::Async<bzd::typeTraits::InvokeResult<decltype(impl::lockGuard::makeLambda<T>), T&>>
{
	co_await lock.lock();
	auto scope = impl::lockGuard::makeLambda(lock);
	co_return bzd::move(scope);
}
} // namespace bzd
