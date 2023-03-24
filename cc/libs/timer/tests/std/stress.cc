#include "cc/bzd/test/multithread.hh"
#include "cc/bzd/test/sync_barrier.hh"
#include "cc/libs/timer/tests/std/timer.hh"

#include <iostream>

// Issue when running:
// bazel test //cc/libs/timer/tests/std:tests --runs_per_test=100 --test_timeout=5 --config=dev --jobs=1 --config=sanitizer --config=tsan

TEST_ASYNC_MULTITHREAD(Timer, Stress, 4)
{
	bzd::test::TimerTest timer{};
	bzd::Atomic<bzd::UInt64> current{0};
	bzd::test::SyncBarrier barrier;

	co_await !timer.init();

	auto sequence1 = [&]() -> bzd::Async<> {
		while (true)
		{
			//::std::cout << current.load() << ::std::endl;
			co_await !timer.waitUntil(current.load());
			co_await !barrier.wait(3);
		}
		co_return {};
	};

	auto sequence2 = [&]() -> bzd::Async<> {
		while (true)
		{
			//::std::cout << current.load() << ::std::endl;
			co_await !timer.waitUntil(current.load());
			co_await !barrier.wait(3);
		}
		co_return {};
	};

	auto sequencer = [&]() -> bzd::Async<> {
		for (bzd::UInt64 t = 0; t < 10; ++t)
		{
			::std::cout << t << ::std::endl;
			current.store(t);
			timer = current.load();
			co_await !barrier.wait(3);
		}
		::std::cout << "TERMINATED" << ::std::endl;
		co_return {};
	};

	bzd::ignore = co_await bzd::async::any(sequencer(), sequence1(), sequence2(), timer.service());

	::std::cout << "FINSIHED" << ::std::endl;

	co_await !timer.shutdown();

	co_return {};
}
