#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/delay.hh"
#include "cc/bzd/test/test.hh"

#include <thread>

template <class Result>
[[nodiscard]] bool isValidResultForAny(Result&& result)
{
	return bzd::apply([](auto&... asyncs) -> bool { return (asyncs.hasValue() || ...); }, bzd::forward<Result>(result));
}

template <bzd::Bool onlyCancellation, bzd::Bool earlyCancellation>
void makeStressCancellationSuspend(const bzd::Size iterations)
{
	for (bzd::Size iteration = 0; iteration < iterations; ++iteration)
	{
		bzd::coroutine::impl::Executor executor{};
		std::thread thread;
		bzd::Atomic<bzd::Bool> shouldReturn{false};
		bzd::async::ExecutableSuspended executableStore{};

		auto workloadSuspend = [&]() -> bzd::Async<> {
			if (earlyCancellation)
			{
				shouldReturn.store(true);
			}
			co_await bzd::async::suspend([&](auto&& executable) {
				thread = std::thread{[&shouldReturn, &executableStore, executable = bzd::move(executable)]() mutable {
					shouldReturn.store(true);
					if constexpr (!onlyCancellation)
					{
						(void)executableStore;
						bzd::move(executable).schedule();
					}
					else
					{
						// Move the executable here to make sure it does not reschedule itself on destruction.
						executableStore = bzd::move(executable);
					}
				}};
			});
			co_return {};
		};

		auto workloadWait = [&]() -> bzd::Async<> {
			while (!shouldReturn.load())
			{
				co_await bzd::async::yield();
			}
			co_return {};
		};

		auto promise1 = workloadSuspend();
		auto promise2 = workloadWait();

		auto promise = bzd::async::any(std::move(promise1), std::move(promise2));
		promise.enqueue(executor);

		// The executor might return as no workload might be in the queue when suspended,
		// so only exit once the promise is ready.
		do
		{
			executor.run();
		} while (!promise.hasResult());

		EXPECT_TRUE(promise.hasResult());
		EXPECT_EQ(executor.getQueueCount(), 0U);
		EXPECT_EQ(executor.getWorkloadCount(), 0);
		EXPECT_TRUE(isValidResultForAny(promise.moveResultOut()));

		thread.join();
	}
}

TEST(Coroutine, StressCancellationSuspend)
{
	makeStressCancellationSuspend<false, false>(1000);
	makeStressCancellationSuspend<false, true>(1000);
}

TEST(Coroutine, StressCancellationSuspendOnlyCancellation)
{
	makeStressCancellationSuspend<true, false>(1000);
	makeStressCancellationSuspend<true, true>(1000);
}
