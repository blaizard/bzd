#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/delay.hh"
#include "cc/bzd/test/test.hh"

#include <thread>

enum class ForkType
{
	any,
	all,
	random
};

template <class Result>
[[nodiscard]] bool isValidResultForAll(Result&& result)
{
	return bzd::apply([](auto&... asyncs) -> bool { return (asyncs.hasValue() && ...); }, bzd::forward<Result>(result));
}

template <class Result>
[[nodiscard]] bool isValidResultForAny(Result&& result)
{
	return bzd::apply([](auto&... asyncs) -> bool { return (asyncs.hasValue() || ...); }, bzd::forward<Result>(result));
}

bzd::Async<> cancellationWorkload(const bzd::Size counter)
{
	for (bzd::Size current = 0; current < counter; ++current)
	{
		co_await bzd::async::yield();
	}
	co_return {};
}

template <ForkType forkType>
bzd::Async<> cancellationNestedWorkload(const bzd::Size counter)
{
	for (bzd::Size current = 0; current < counter; ++current)
	{
		auto promise1 = cancellationWorkload(counter);
		auto promise2 = cancellationNestedWorkload<forkType>(counter / 2);
		auto promise3 = cancellationNestedWorkload<forkType>(counter / 3);
		if constexpr (forkType == ForkType::any)
		{
			auto result = co_await bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
			EXPECT_TRUE(isValidResultForAny(result));
		}
		else if constexpr (forkType == ForkType::all)
		{
			auto result = co_await bzd::async::all(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
			EXPECT_TRUE(isValidResultForAll(result));
		}
		else if constexpr (forkType == ForkType::random)
		{
			if (counter % 2)
			{
				auto result = co_await bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
				EXPECT_TRUE(isValidResultForAny(result));
			}
			else
			{
				auto result = co_await bzd::async::all(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
				EXPECT_TRUE(isValidResultForAll(result));
			}
		}
		else
		{
			bzd::assert::unreachable();
		}
	}
	co_return {};
}

template <ForkType forkType, class Function>
void spawnConcurrentThreads(bzd::Async<> (*workload)(const bzd::Size), const bzd::Size iterations, const Function counterGenerator)
{
	for (bzd::Size iteration = 0; iteration < iterations; ++iteration)
	{
		bzd::coroutine::impl::Executor executor{};
		bzd::Array<std::thread, 10> threads;

		auto promise1 = workload(counterGenerator());
		auto promise2 = workload(counterGenerator());
		auto promise3 = workload(counterGenerator());
		auto promise4 = workload(counterGenerator());
		auto promise5 = workload(counterGenerator());

		auto execute = [&](auto& promise) {
			promise.enqueue(executor);
			for (auto& entry : threads)
			{
				entry = std::thread{[&executor]() { executor.run(); }};
			}

			for (auto& entry : threads)
			{
				entry.join();
			}

			EXPECT_TRUE(promise.isReady());
		};

		if constexpr (forkType == ForkType::any)
		{
			auto promise =
				bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3), bzd::move(promise4), bzd::move(promise5));
			execute(promise);
			EXPECT_TRUE(isValidResultForAny(promise.moveResultOut().value()));
		}
		else if constexpr (forkType == ForkType::all)
		{
			auto promise =
				bzd::async::all(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3), bzd::move(promise4), bzd::move(promise5));
			execute(promise);
			EXPECT_TRUE(isValidResultForAll(promise.moveResultOut().value()));
		}
		else if constexpr (forkType == ForkType::random)
		{
			if (static_cast<bzd::Bool>(counterGenerator() % 2))
			{
				auto promise = bzd::async::any(bzd::move(promise1),
											   bzd::move(promise2),
											   bzd::move(promise3),
											   bzd::move(promise4),
											   bzd::move(promise5));
				execute(promise);
				EXPECT_TRUE(isValidResultForAny(promise.moveResultOut().value()));
			}
			else
			{
				auto promise = bzd::async::all(bzd::move(promise1),
											   bzd::move(promise2),
											   bzd::move(promise3),
											   bzd::move(promise4),
											   bzd::move(promise5));
				execute(promise);
				EXPECT_TRUE(isValidResultForAll(promise.moveResultOut().value()));
			}
		}
		else
		{
			bzd::assert::unreachable();
		}

		EXPECT_EQ(executor.getQueueCount(), 0U);
		EXPECT_EQ(executor.getWorkloadCount(), 0);
	}
}

TEST(Coroutine, StressAllNull)
{
	spawnConcurrentThreads<ForkType::all>(cancellationWorkload, 1000, []() { return 0; });
}

TEST(Coroutine, StressAnyNull)
{
	spawnConcurrentThreads<ForkType::any>(cancellationWorkload, 1000, []() { return 0; });
}

TEST(Coroutine, StressAllFixed)
{
	spawnConcurrentThreads<ForkType::all>(cancellationWorkload, 1000, []() { return 10; });
}

TEST(Coroutine, StressAnyFixed)
{
	spawnConcurrentThreads<ForkType::any>(cancellationWorkload, 1000, []() { return 10; });
}

TEST(Coroutine, StressAllRandom)
{
	spawnConcurrentThreads<ForkType::all>(cancellationWorkload, 1000, [&]() { return test.random<int, 0, 100>(); });
}

TEST(Coroutine, StressAnyRandom)
{
	spawnConcurrentThreads<ForkType::any>(cancellationWorkload, 1000, [&]() { return test.random<int, 0, 100>(); });
}

TEST(Coroutine, StressAllNested)
{
	spawnConcurrentThreads<ForkType::all>(cancellationNestedWorkload<ForkType::all>, 100, [&]() { return test.random<int, 0, 10>(); });
}

TEST(Coroutine, StressAnyNested)
{
	spawnConcurrentThreads<ForkType::any>(cancellationNestedWorkload<ForkType::any>, 100, [&]() { return test.random<int, 0, 10>(); });
}

TEST(Coroutine, StressAllAnyNested)
{
	spawnConcurrentThreads<ForkType::all>(cancellationNestedWorkload<ForkType::any>, 100, [&]() { return test.random<int, 0, 10>(); });
}

TEST(Coroutine, StressAnyAllNested)
{
	spawnConcurrentThreads<ForkType::any>(cancellationNestedWorkload<ForkType::all>, 100, [&]() { return test.random<int, 0, 10>(); });
}

TEST(Coroutine, StressRandomNested)
{
	spawnConcurrentThreads<ForkType::random>(cancellationNestedWorkload<ForkType::random>, 100, [&]() {
		return test.random<int, 0, 10>();
	});
}

TEST(Coroutine, StressCancellationSuspend)
{
	constexpr bzd::Size iterations = 10000;

	for (bzd::Size iteration = 0; iteration < iterations; ++iteration)
	{
		bzd::coroutine::impl::Executor executor{};
		std::thread thread;
		bzd::Atomic<bzd::Bool> shouldReturn{false};

		//::std::cout << iteration << ::std::endl;

		using namespace std::chrono_literals;

		auto workloadSuspend = [&]() -> bzd::Async<> {
			co_await bzd::async::suspend([&](auto&& executable) {
				thread = std::thread{[&shouldReturn, executable = bzd::move(executable)]() mutable {
					shouldReturn.store(true);
					bzd::move(executable).schedule();
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
			if (!promise.isReady())
			{
				//	std::this_thread::sleep_for(100ms);
			}
		} while (!promise.isReady());

		EXPECT_TRUE(promise.isReady());
		EXPECT_EQ(executor.getQueueCount(), 0U);
		EXPECT_EQ(executor.getWorkloadCount(), 0);
		EXPECT_TRUE(isValidResultForAny(promise.moveResultOut().value()));

		thread.join();
	}
}
