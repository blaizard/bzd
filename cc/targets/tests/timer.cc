#include "cc/targets/tests/timer.hh"

#include "cc/bzd/test/test.hh"

TEST_ASYNC(Timer, Basic)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;
	co_await !timer.delay(bzd::units::Millisecond{10});

	co_return {};
}

TEST_ASYNC(Timer, Delay)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;
	bzd::Atomic<bzd::Size> nbYields{0u};
	auto yieldCounter = [&nbYields]() -> bzd::Async<> {
		while (true)
		{
			++nbYields;
			co_await bzd::async::yield();
		}
		co_return {};
	};

	// Check that it yields at least several times.
	co_await !bzd::async::any(timer.delay(bzd::units::Millisecond{10}), yieldCounter());
	EXPECT_GT(nbYields.load(), 0u);

	co_return {};
}

TEST_ASYNC(Timer, Timeout)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;

	// Check that the timeout works as expected.
	const auto result = co_await bzd::async::any(timer.delay(bzd::units::Second{1000}),
												 timer.timeout(bzd::units::Millisecond{10}),
												 timer.delay(bzd::units::Second{1000}));
	EXPECT_TRUE(result.template get<0>().empty());
	EXPECT_TRUE(result.template get<1>().hasValue());
	const auto& value = result.template get<1>().value();
	EXPECT_FALSE(value.hasValue());

	co_return {};
}
