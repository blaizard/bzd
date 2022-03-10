#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/delay.hh"
#include "cc/bzd/test/test.hh"

#include <thread>

bzd::Async<> cancellationWorkload(const bzd::SizeType counter)
{
	for (bzd::SizeType current = 0; current < counter; ++current)
	{
		co_await bzd::async::yield();
	}
	co_return {};
}

bzd::Async<> cancellationNestedWorkload(const bzd::SizeType counter)
{
	for (bzd::SizeType current = 0; current < counter; ++current)
	{
		auto promise1 = cancellationWorkload(counter);
		auto promise2 = cancellationNestedWorkload(counter / 2);
		auto promise3 = cancellationNestedWorkload(counter / 3);
		co_await bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
	}
	co_return {};
}

TEST(Coroutine, Cancellation2Threads)
{
	bzd::impl::AsyncExecutor executor;
	bzd::Array<std::thread, 2> threads;

	auto promise1 = cancellationWorkload(2);
	auto promise2 = cancellationWorkload(1);
	auto promise = bzd::async::any(bzd::move(promise1), bzd::move(promise2));
	promise.enqueue(executor);

	for (auto& entry : threads)
	{
		entry = std::thread{[&executor]() { executor.run(); }};
	}

	for (auto& entry : threads)
	{
		entry.join();
	}

	EXPECT_EQ(executor.getQueueCount(), 0U);
}

template <class Function>
void spawnConcurrentThreads(bzd::Async<> (*workload)(const bzd::SizeType), const bzd::SizeType iterations, const Function counterGenerator)
{
	for (bzd::SizeType iteration = 0; iteration < iterations; ++iteration)
	{
		bzd::impl::AsyncExecutor executor{};
		bzd::Array<std::thread, 10> threads;

		auto promise1 = workload(counterGenerator());
		auto promise2 = workload(counterGenerator());
		auto promise3 = workload(counterGenerator());
		auto promise4 = workload(counterGenerator());
		auto promise5 = workload(counterGenerator());
		auto promise =
			bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3), bzd::move(promise4), bzd::move(promise5));
		promise.enqueue(executor);

		for (auto& entry : threads)
		{
			entry = std::thread{[&executor]() { executor.run(); }};
		}

		for (auto& entry : threads)
		{
			entry.join();
		}

		EXPECT_EQ(executor.getQueueCount(), 0U);
	}
}

TEST(Coroutine, CancellationStressNull)
{
	spawnConcurrentThreads(cancellationWorkload, 1000, []() { return 0; });
}

TEST(Coroutine, CancellationStressFixed)
{
	spawnConcurrentThreads(cancellationWorkload, 1000, []() { return 10; });
}

TEST(Coroutine, CancellationStressRandom)
{
	spawnConcurrentThreads(cancellationWorkload, 1000, [&]() { return test.random<int, 0, 1000>(); });
}

TEST(Coroutine, CancellationStressNested)
{
	spawnConcurrentThreads(cancellationNestedWorkload, 1000, [&]() { return test.random<int, 0, 10>(); });
}

/*
TEST_ASYNC(Coroutine, fibonacci)
{
	auto promiseA = bzd::delay(10_ms);
	auto promiseB = bzd::delay(100_s);

	auto promiseC = bzd::async::any(promiseA, promiseB);

	auto promiseD = bzd::delay(1_s);

	co_await bzd::async::any(promiseC, promiseD);

	//EXPECT_TRUE(false);

	co_return {};
}
*/
