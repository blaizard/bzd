#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::async::awaitable {

template <class Async>
class Awaiter
{
public:
	constexpr explicit Awaiter(Async& async) noexcept : async_{async} {}

	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_ready() noexcept { return false; }

	template <class U>
	// NOLINTNEXTLINE(readability-identifier-naming)
	constexpr bool await_suspend(bzd::async::impl::coroutine_handle<U> caller) noexcept
	{
		bzd::assert::isTrue(static_cast<bool>(async_.handle_));
		// caller ()
		// {
		//    co_await this ()
		/// }
		auto& promise = async_.handle_.promise();
		auto& promiseCaller = caller.promise();

		// Propagate the context of the caller to this async.
		promiseCaller.propagate(promise);

		// Handle the continuation.
		promise.setContinuation(promiseCaller);

		// Enqueue the current handle to the executor.
		promiseCaller.thenEnqueueExecutable(promise);

		// Returns control to the caller of the current coroutine,
		// as the current coroutine is already queued for execution.
		return true;
	}

	// NOLINTNEXTLINE(readability-identifier-naming)
	[[nodiscard]] constexpr auto await_resume() noexcept { return async_.moveResultOut(); }

protected:
	Async& async_;
};

} // namespace bzd::async::awaitable
