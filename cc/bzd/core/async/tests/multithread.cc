#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/delay.hh"
#include "cc/bzd/test/test.hh"

#include <thread>

bzd::Async<> cancellationWorkload(const auto duration)
{
	::std::this_thread::sleep_for(duration);
	co_return {};
}

TEST(Coroutine, Cancellation2Threads)
{
	using namespace std::chrono_literals;
	bzd::impl::AsyncExecutor executor;
	bzd::Array<std::thread, 2> threads;

	auto promise1 = cancellationWorkload(2ms);
	auto promise2 = cancellationWorkload(1ms);
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
	using namespace std::chrono_literals;

	for (bzd::SizeType iteration = 0; iteration < 1000; ++iteration)
	{
		bzd::impl::AsyncExecutor executor{};
		bzd::Array<std::thread, 5> threads;

		auto promise1 = cancellationWorkload(1ms);
		auto promise2 = cancellationWorkload(1ms);
		auto promise3 = cancellationWorkload(1ms);
		auto promise4 = cancellationWorkload(1ms);
		auto promise5 = cancellationWorkload(1ms);
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
