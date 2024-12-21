#pragma once

#include "cc/bzd/core/async/awaitables/default.hh"

namespace bzd::async::awaitable {

template <class Async>
class AwaiterNoReturn : public Awaiter<Async>
{
public:
	using Awaiter<Async>::Awaiter;

	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr void await_resume() noexcept {}
};

} // namespace bzd::async::awaitable
