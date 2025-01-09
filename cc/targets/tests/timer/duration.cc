#include "cc/bzd/test/test.hh"
#include "cc/targets/tests/timer/runner.hh"

TEST_ASYNC(Timer, Duration)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;
	co_await !timer.delay(bzd::units::Second{1});

	co_return {};
}
