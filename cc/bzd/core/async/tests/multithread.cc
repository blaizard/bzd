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

TEST(Coroutine, Cancellation2Threads)
{
	bzd::impl::AsyncExecutor executor;
	bzd::Array<std::thread, 2> threads;

	auto promise1 = cancellationWorkload(2);
	auto promise2 = cancellationWorkload(1);
	auto promise = bzd::async::any(promise1, promise2);
	promise.enqueue(executor);

	for (auto& entry : threads)
	{
		entry = std::thread{[&executor]() { executor.run(); }};
	}

	for (auto& entry : threads)
	{
		entry.join();
	}
}

TEST(Coroutine, CancellationStress)
{
	for (bzd::SizeType iteration = 0; iteration < 1000; ++iteration)
	{
		bzd::impl::AsyncExecutor executor{};
		bzd::Array<std::thread, 5> threads;

		auto promise1 = cancellationWorkload(0);
		auto promise2 = cancellationWorkload(0);
		auto promise3 = cancellationWorkload(0);
		auto promise4 = cancellationWorkload(0);
		auto promise5 = cancellationWorkload(0);
		auto promise = bzd::async::any(promise1, promise2, promise3, promise4, promise5);
		promise.enqueue(executor);

		for (auto& entry : threads)
		{
			entry = std::thread{[&executor]() { executor.run(); }};
		}

		for (auto& entry : threads)
		{
			entry.join();
		}
	}
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
