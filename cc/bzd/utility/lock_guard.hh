#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd {
template <class T>
auto makeLambda(T& lock)
{
	auto scope = bzd::ScopeGuard([&lock]() { lock.unlock(); });
	return bzd::move(scope);
}

template <class T>
auto makeLockGuard(T& lock) -> bzd::Async<bzd::typeTraits::InvokeResult<decltype(bzd::makeLambda<T>), T&>>
{
	co_await lock.lock();
	auto scope = makeLambda(lock);
	co_return bzd::move(scope);
}
} // namespace bzd
