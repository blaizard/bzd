#include "cc/bzd/test/multithread.hh"
#include "cc/bzd/test/sync_barrier.hh"
#include "cc/libs/timer/tests/std/timer.hh"

TEST_ASYNC_MULTITHREAD(Timer, Stress, 4)
{
	bzd::test::TimerTest timer{};
	bzd::Atomic<bzd::UInt64> current{0};
	bzd::test::SyncBarrier barrier;

	co_await !timer.init();

	auto sequence1 = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await !timer.waitUntil(current.load());
			co_await !barrier.wait(3);
		}
		co_return {};
	};

	auto sequence2 = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await !timer.waitUntil(current.load());
			co_await !barrier.wait(3);
		}
		co_return {};
	};

	auto sequencer = [&]() -> bzd::Async<> {
		for (auto t = 0; t < 10; ++t)
		{
			current.store(t);
			timer = current.load();
			co_await !barrier.wait(3);
		}
		co_return {};
	};

	bzd::ignore = co_await bzd::async::any(sequencer(), sequence1(), sequence2(), timer.service());

	co_await !timer.shutdown();

	co_return {};
}
