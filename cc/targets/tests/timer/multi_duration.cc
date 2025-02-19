#include "cc/bzd/test/test.hh"
#include "cc/targets/tests/timer/runner.hh"

TEST_ASYNC(Timer, Duration)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;
	const auto result = co_await bzd::async::all(timer.delay(bzd::units::Second{5}),
												 timer.delay(bzd::units::Millisecond{2}),
												 timer.delay(bzd::units::Millisecond{100}));

	EXPECT_TRUE(result.template get<0>().hasValue());
	EXPECT_TRUE(result.template get<1>().hasValue());
	EXPECT_TRUE(result.template get<2>().hasValue());

	co_return {};
}
