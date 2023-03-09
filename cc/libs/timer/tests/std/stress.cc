#include "cc/bzd/test/multithread.hh"
#include "cc/libs/timer/tests/std/timer.hh"

TEST_ASYNC_MULTITHREAD(Timer, Stress, 4)
{
	bzd::test::TimerTest timer{};
	bzd::Atomic<bzd::UInt64> current{0};
	bzd::Atomic<bzd::Size> sync{0};

	co_await !timer.init();

	auto sequence1 = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await !timer.waitUntil(current.load());
			++sync;
			while (sync.load() != 0)
			{
				co_await bzd::async::yield();
			}
		}
		co_return {};
	};

	auto sequence2 = [&]() -> bzd::Async<> {
		while (true)
		{
			co_await !timer.waitUntil(current.load());
			++sync;
			while (sync.load() != 0)
			{
				co_await bzd::async::yield();
			}
		}
		co_return {};
	};

	auto sequencer = [&]() -> bzd::Async<> {
		for (bzd::UInt64 t = 0; t < 100; ++t)
		{
			current.store(t);
			timer = current.load();
			// Wait for the wait until to be completed.
			while (sync.load() < 2)
			{
				co_await bzd::async::yield();
			}
			sync.store(0);
		}
		co_return {};
	};

	bzd::ignore = co_await bzd::async::any(sequencer(), sequence1(), sequence2(), timer.service());

	co_await !timer.shutdown();

	co_return {};
}
