#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/stream.hh"
#include "cc/bzd/utility/format/stream.hh"

namespace bzd {
/// Helper to print a message on the standard output.
///
/// \param[in] args... All arguments are forwarded to the toStream function.
template <class... Args>
bzd::Async<> print(Args&&... args) noexcept;

/// Helper to print a message on the standard output without holding the lock.
///
/// \param[in] args... All arguments are forwarded to the toStream function.
template <class... Args>
bzd::Async<> printNoLock(Args&&... args) noexcept;
} // namespace bzd

template <class... Args>
bzd::Async<> bzd::print(Args&&... args) noexcept
{
	auto& out = bzd::platform::out();
	auto scope = co_await out.getLock();
	co_await !toStream(out, bzd::forward<Args>(args)...);
	co_return {};
}

template <class... Args>
bzd::Async<> bzd::printNoLock(Args&&... args) noexcept
{
	auto& out = bzd::platform::out();
	co_await !toStream(out, bzd::forward<Args>(args)...);
	co_return {};
}
