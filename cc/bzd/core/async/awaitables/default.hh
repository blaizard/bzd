#pragma once

#include "cc/bzd/core/async/promise.hh"

namespace bzd::coroutine::impl {

template <class Async>
class Awaiter
{
public:
	constexpr explicit Awaiter(Async& async) noexcept : async_{async} {}

	constexpr bool await_ready() noexcept { return async_.isCompleted(); }

	template <class U>
	constexpr bool await_suspend(bzd::coroutine::impl::coroutine_handle<U> caller) noexcept
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

	[[nodiscard]] constexpr auto await_resume() noexcept
	{
		return async_.moveResultOut();
	}

private:
	Async& async_;
};

} // namespace bzd::coroutine::impl
