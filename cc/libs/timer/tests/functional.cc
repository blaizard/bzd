#include "cc/bzd/test/test.hh"
#include "cc/libs/timer/tests/impl/timer.hh"

TEST_ASYNC(Timer, Simple)
{
	bzd::test::TimerTest timer{};

	co_await !timer.init();

	auto sequence = []() -> bzd::Async<> { co_return {}; };

	co_await !bzd::async::any(sequence(), timer.service());

	co_await !timer.shutdown();

	co_return {};
}
