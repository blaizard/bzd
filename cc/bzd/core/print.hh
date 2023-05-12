#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/pattern/to_stream.hh"

namespace bzd {
/// Helper to print a message on the standard output.
///
/// \param[in] args... All arguments are forwarded to the toStream function.
template <class... Args>
bzd::Async<> print(bzd::OStream& out, Args&&... args) noexcept
{
	auto scope = co_await out.getLock();
	co_await !toStream(out, bzd::forward<Args>(args)...);
	co_return {};
}

/// Helper to print a message on the standard output without holding the lock.
///
/// \param[in] args... All arguments are forwarded to the toStream function.
template <class... Args>
bzd::Async<> printNoLock(bzd::OStream& out, Args&&... args) noexcept
{
	co_await !toStream(out, bzd::forward<Args>(args)...);
	co_return {};
}

} // namespace bzd
