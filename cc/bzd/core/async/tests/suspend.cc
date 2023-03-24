#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/delay.hh"
#include "cc/bzd/test/multithread.hh"
#include "cc/bzd/test/sync_barrier.hh"
/*
template <class Result>
[[nodiscard]] bool isValidResultForAny(Result&& result)
{
	return bzd::apply([](auto&... asyncs) -> bool { return (asyncs.hasValue() || ...); }, bzd::forward<Result>(result));
}

template <bzd::Bool useISR, bzd::Bool onlyCancellation, bzd::Bool earlyCancellation>
void makeStressCancellationSuspend(const bzd::Size iterations)
{
	using ExecutableSuspended =
		bzd::typeTraits::Conditional<useISR, bzd::async::ExecutableSuspendedForISR, bzd::async::ExecutableSuspended>;

	for (bzd::Size iteration = 0; iteration < iterations; ++iteration)
	{
		bzd::coroutine::impl::Executor executor{};
		std::thread thread;
		bzd::Atomic<bzd::Bool> shouldReturn{false};
		ExecutableSuspended executableStore{};

		auto workloadSuspend = [&]() -> bzd::Async<> {
			if (earlyCancellation)
			{
				shouldReturn.store(true);
			}
			auto onSuspend = [&](auto&& executable) {
				executableStore.own(bzd::move(executable));
				thread = std::thread{[&shouldReturn, &executableStore]() mutable {
					shouldReturn.store(true);
					if constexpr (!onlyCancellation)
					{
						bzd::move(executableStore).schedule();
					}
					else
					{
						(void)executableStore;
					}
				}};
			};
			if constexpr (useISR)
			{
				co_await bzd::async::suspendForISR(bzd::move(onSuspend));
			}
			else
			{
				co_await bzd::async::suspend(bzd::move(onSuspend));
			}
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
	makeStressCancellationSuspend<false, false, false>(1000);
	makeStressCancellationSuspend<false, false, true>(1000);
}

TEST(Coroutine, StressCancellationSuspendOnlyCancellation)
{
	makeStressCancellationSuspend<false, true, false>(1000);
	makeStressCancellationSuspend<false, true, true>(1000);
}

TEST(Coroutine, StressCancellationSuspendForISR)
{
	makeStressCancellationSuspend<true, false, false>(1000);
	makeStressCancellationSuspend<true, false, true>(1000);
}

TEST(Coroutine, StressCancellationSuspendForISROnlyCancellation)
{
	makeStressCancellationSuspend<true, true, false>(1000);
	makeStressCancellationSuspend<true, true, true>(1000);
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressSuspend, 3)
{
	bzd::async::ExecutableSuspended executable{};
	bzd::test::SyncBarrier barrier;

	auto sequence1 = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await !barrier.wait(3, [&]() { executable.schedule(); });
		}
		co_return {};
	};

	auto sequence2 = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await !barrier.wait(3, [&]() { executable.schedule(); });
		}
		co_return {};
	};

	auto sequencer = [&]() -> bzd::Async<> {
		for (bzd::UInt64 t = 0; t < 1000; ++t)
		{
			co_await bzd::async::suspend([&](auto&& suspended) { executable.own(bzd::move(suspended)); });
			// Wait for the wait until to be completed.
			co_await !barrier.wait(3);
		}
		co_return {};
	};

	bzd::ignore = co_await bzd::async::any(sequencer(), sequence1(), sequence2());

	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressSuspendCancellation, 2)
{
	bzd::async::ExecutableSuspended executable{};
	bzd::test::SyncBarrier barrier;

	auto schedule = [&]() -> bzd::Async<> {
		for (auto t = 0; t < 2; ++t)
		{
			co_await !barrier.wait(2, [&]() { executable.schedule(); });
		}
		co_return {};
	};

	auto suspend = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await bzd::async::suspend([&](auto&& suspended) { executable.own(bzd::move(suspended)); });
			co_await !barrier.wait(2);
		}
		co_return {};
	};

	bzd::ignore = co_await bzd::async::any(suspend(), schedule());

	::std::cout << "AFTER" << ::std::endl;

	co_return {};
}

