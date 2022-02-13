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

TEST(Coroutine, Cancellation)
{
	using namespace std::chrono_literals;
	bzd::impl::AsyncExecutor executor;
	bzd::Array<std::thread, 2> threads;

	auto promise1 = cancellationWorkload(2ms);
	auto promise2 = cancellationWorkload(1ms);
	auto promise = bzd::async::any(promise1, promise2);
	promise.enqueue(executor);

	// it crashes because:
	// thread 1                                   thread 2
	//  done -> continuation [any]               executing -> continuation [any]
	// the continuation [any] is used                [any] is pushed to the list.
	//  [any] terminates                            [any] is poped and used /!\ crash!

	for (auto& entry : threads)
	{
		entry = std::thread{[&executor]() {
			executor.run();
			::std::cout << "end" << ::std::endl;
		}};
	}

	for (auto& entry : threads)
	{
		entry.join();
	}

	::std::cout << "after join" << ::std::endl;
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
